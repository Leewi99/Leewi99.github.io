// Tarvittavat kirjastot ESP8266:n WiFi-yhteyksiin, HTTP-pyyntöihin, turvallisiin yhteyksiin, JSONin käsittelyyn, I2C-väylään ja OLED-näyttöjen ohjaukseen
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>

#define OLED_RESET -1
Adafruit_SSD1306 display1(128, 64, &Wire, OLED_RESET);
Adafruit_SSD1306 display2(128, 64, &Wire, OLED_RESET);

// WiFi-verkon asetukset ja NTP-palvelimen tiedot

const char* ssid = "Wifi-network-name";
const char* password = "Wifi-password";
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -3600;
const int daylightOffset_sec = 3600;

// Luodaan WiFiClientSecure-objekti HTTPS-yhteyttä varten
WiFiClientSecure client;

void setup() {
  // Alustaa sarjaportin, yhdistää WiFi-verkkoon ja odottaa yhteyttä
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // Asettaa ajan NTP-palvelimelta ja odottaa, kunnes aika on saatavilla
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nTime synchronized");

  // Alustaa I2C-väylän ja molemmat OLED-näytöt ja tyhjentää näytön
  Wire.begin();
  display1.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display1.display();
  delay(2000);
  display1.clearDisplay();

  display2.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display2.display();
  delay(2000);
  display2.clearDisplay();
}

void loop() {
  // Hakee ja näyttää sähkön hinnan, päivittää tiedot joka minuutti
  fetchAndDisplayElectricityPrice();
  delay(60000);
}
// Valitsee I2C-väylän PCA9548A-multiplexerin kautta
void PCA9548A(uint8_t bus) {
  Wire.beginTransmission(0x70);
  Wire.write(1 << bus);
  Wire.endTransmission();
}
void initDisplays() {
  PCA9548A(7);
  if (!display1.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Display 1 init failed");
    for (;;); // Infinite loop to halt further execution
  }
  display1.clearDisplay();

  PCA9548A(2);
  if (!display2.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Display 2 init failed");
    for (;;);
  }
  display2.clearDisplay();
}

// Hakee ja näyttää sähkön hinnan käyttäen API-kutsua
void fetchAndDisplayElectricityPrice() {
  // Tarkistaa WiFi-yhteyden tilan
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    client.setInsecure(); // Ohittaa SSL-varmenteen tarkistuksen
    http.begin(client, "https://api.porssisahko.net/v1/latest-prices.json");
    int httpCode = http.GET();

    // Onnistuneen HTTP-pyynnön käsittely
    if (httpCode == 200) {
      String payload = http.getString();
      DynamicJsonDocument doc(2048);
      deserializeJson(doc, payload);
      JsonArray prices = doc["prices"];

      time_t now;
      struct tm timeinfo;
      time(&now);
      localtime_r(&now, &timeinfo);

      float currentPrice = 0.0;
      bool currentPriceFound = false;
      for(JsonVariant p : prices) {
    float price = p["price"];
    const char* startDateStr = p["startDate"];

    struct tm start = {0};
    strptime(startDateStr, "%Y-%m-%dT%H:%M:%S.000Z", &start);

    // Ajan muuntaminen ottaen huomioon GMT-offset ja kesäaika
    time_t startEpoch = mktime(&start) - gmtOffset_sec - daylightOffset_sec;
    
    localtime_r(&startEpoch, &start); // Muuntaa epoch-ajan takaisin struct tm:ksi paikallisessa aikavyöhykkeessä

    if (start.tm_hour == timeinfo.tm_hour && start.tm_mday == timeinfo.tm_mday && start.tm_mon == timeinfo.tm_mon) {
      currentPrice = price;
      currentPriceFound = true;
      break;
    }
}

      if (currentPriceFound) {
        Serial.print("Nykyinen aika: ");
        timeinfo.tm_hour += 3;
        if(timeinfo.tm_hour < 10) Serial.print('0');
        Serial.print(timeinfo.tm_hour);
        Serial.print(":");
        if(timeinfo.tm_min < 10) Serial.print('0');
        Serial.print(timeinfo.tm_min);
        Serial.print(" Nykyinen hinta: ");
        Serial.print(currentPrice, 2);
        Serial.println(" c/kWh");
        displayPriceOnOLED(currentPrice, timeinfo);
      } else {
        Serial.println("Nykyisen tunnin hintaa ei löydetty.");
      }
      
      drawPriceChartOnOLED2(prices);
    } else {
      Serial.print("HTTP GET request failed, error code: ");
      Serial.println(httpCode);
    }
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}

// Näyttää ajan ja hinnan ensimmäisellä OLED-näytöllä
void displayPriceOnOLED(float price, struct tm timeinfo) {
  PCA9548A(7); // Valitsee I2C-väylän
  display1.clearDisplay();
  display1.setTextSize(2);
  display1.setTextColor(WHITE);
  display1.setCursor(0, 0);
  if(timeinfo.tm_hour < 10) display1.print('0');
  display1.print(timeinfo.tm_hour);
  display1.print(":");
  if(timeinfo.tm_min < 10) display1.print('0');
  display1.print(timeinfo.tm_min);
  display1.setTextSize(3);
  display1.setCursor(0, 20);
  display1.print(price, 2);
  display1.setTextSize(2);
  display1.setCursor(0, 50);
  display1.println("c/kWh");
  display1.display();
}

// Näyttää pylväsdiagrammin hinnoista
void drawPriceChartOnOLED2(JsonArray& prices) {
    PCA9548A(2);
    display2.clearDisplay();
    display2.setTextSize(1);
    display2.setTextColor(WHITE);
    const int maxHeight = 45;
    float maxPrice = 0.0; 
    const int width = 3;
    const int gap = 2;
    const int baseLine = 54;

    time_t now;
    time(&now);
    struct tm now_tm;
    localtime_r(&now, &now_tm);

    for (JsonVariant p : prices) {
        float price = p["price"];
        const char* startDateStr = p["startDate"];
        struct tm start_tm = {0};
        strptime(startDateStr, "%Y-%m-%dT%H:%M:%S.000Z", &start_tm);
        if (start_tm.tm_year == now_tm.tm_year && start_tm.tm_mon == now_tm.tm_mon && start_tm.tm_mday == now_tm.tm_mday) {
            if (price > maxPrice) maxPrice = price;
        }
    }

    int x = 0; // Aloitetaan piirtäminen vasemmasta reunasta

    for (int hour = 0; hour < 24; hour++) {
        bool priceFound = false;
        float priceForHour = 0.0;
        
        for (JsonVariant p : prices) {
            const char* startDateStr = p["startDate"];
            struct tm start_tm = {0};
            strptime(startDateStr, "%Y-%m-%dT%H:%M:%S.000Z", &start_tm);
            if (start_tm.tm_year == now_tm.tm_year && start_tm.tm_mon == now_tm.tm_mon && start_tm.tm_mday == now_tm.tm_mday && start_tm.tm_hour == hour) {
                priceForHour = p["price"];
                priceFound = true;
                break;
            }
        }
        
        // Piirretään palkki vain, jos hinta löytyi kyseiselle tunnille
        if (priceFound) {
            int height = (int)((priceForHour / maxPrice) * maxHeight);
            display2.fillRect(x, baseLine - height, width, height, WHITE);
        } else {
        }

        // Kellonajat piirretään joka kolmas tunti
        if (hour % 3 == 0) {
            display2.setCursor(x, baseLine + 2);
            if(hour < 10) display2.print('0');
            display2.print(hour);
            display2.print(" ");
        }

        x += width + gap;
    }

    display2.display();
}
