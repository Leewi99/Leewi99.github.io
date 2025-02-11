package application.controller;

import java.io.IOException;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.ProgressBar;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
import java.util.Random;

public class MainViewController {

    @FXML private Label TulosLabel;
    @FXML private Label ValintaLabel;
    @FXML private ProgressBar progressBar;
    @FXML private ProgressBar progressBar2;
    @FXML private Label PelaajanValintaLabel;
    @FXML private Label TietokoneenValintaLabel;
    @FXML private Button KiviButton;
    @FXML private Button PaperiButton;
    @FXML private Button SaksetButton;

    private int pelaajanVoitot = 0;
    private int tietokoneenVoitot = 0;
    private int voittojaVoittoon = 3;

    private final Random random = new Random();

    @FXML
    private void handleKiviClick() {
        pelaaKierros("Kivi");
    }

    @FXML
    private void handlePaperiClick() {
        pelaaKierros("Paperi");
    }

    @FXML
    private void handleSaksetClick() {
        pelaaKierros("Sakset");
    }

    private void pelaaKierros(String pelaajanValinta) {
        String[] valinnat = {"Kivi", "Paperi", "Sakset"};
        String tietokoneenValinta = valinnat[random.nextInt(3)];

        PelaajanValintaLabel.setText("Pelaaja: " + pelaajanValinta);
        TietokoneenValintaLabel.setText("Tietokone: " + tietokoneenValinta);

        String tulos = ratkaiseVoittaja(pelaajanValinta, tietokoneenValinta);
        
        if (tulos.equals("Pelaaja voitti!")) {
            pelaajanVoitot++;
        } else if (tulos.equals("Tietokone voitti!")) {
            tietokoneenVoitot++;
        }

        TulosLabel.setText("Tulos: " + tulos + "\nPelaaja: " + pelaajanVoitot + " - Tietokone: " + tietokoneenVoitot);

        progressBar.setProgress((double) pelaajanVoitot / voittojaVoittoon);
        progressBar2.setProgress((double) tietokoneenVoitot / voittojaVoittoon);

        if (pelaajanVoitot >= voittojaVoittoon) {
            PeliLoppuRuutu("Onneksi olkoon! \nVoitit pelin!");
            lukitsePainikkeet();
        } else if (tietokoneenVoitot >= voittojaVoittoon) {
            PeliLoppuRuutu("Tietokone voitti pelin! \nYritä uudelleen.");
            lukitsePainikkeet();
        }
    }

    private String ratkaiseVoittaja(String pelaaja, String tietokone) {
        if (pelaaja.equals(tietokone)) {
            return "Tasapeli!";
        }
        if ((pelaaja.equals("Kivi") && tietokone.equals("Sakset")) ||
            (pelaaja.equals("Paperi") && tietokone.equals("Kivi")) ||
            (pelaaja.equals("Sakset") && tietokone.equals("Paperi"))) {
            return "Pelaaja voitti!";
        } else {
            return "Tietokone voitti!";
        }
    }

    private void PeliLoppuRuutu(String tulos) {
        try {
            FXMLLoader loader = new FXMLLoader(getClass().getResource("/application/view/GameOverView.fxml"));
            Parent gameOverViewRoot = loader.load();

            GameOverViewController gameOverController = loader.getController();
            gameOverController.pelinTulos(tulos);

            Stage stage = new Stage();
            Scene scene = new Scene(gameOverViewRoot);

            String css = getClass().getResource("/application/application.css").toExternalForm();
            scene.getStylesheets().add(css);
            stage.setScene(scene);
            stage.show();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void lukitsePainikkeet() {
        KiviButton.setDisable(true);
        PaperiButton.setDisable(true);
        SaksetButton.setDisable(true);
    }

    public void asetaVoittojenMaara(int maara) {
        this.voittojaVoittoon = maara;
        System.out.println("Voittojen määrä asetettu: " + maara);
    }
}