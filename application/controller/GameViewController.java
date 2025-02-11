package application.controller;

import java.io.IOException;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.RadioButton;
import javafx.scene.control.ToggleGroup;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

public class GameViewController {

    @FXML private Label GameLabel;
    @FXML private Label KierroksetLabel;
    @FXML private RadioButton KolmeButton;
    @FXML private RadioButton ViisiButton;
    @FXML private RadioButton YksiButton;
    @FXML private Button PelaaButton;

    private int tarvittavatVoitot = 1;
    private ToggleGroup toggleGroup;

    @FXML public void initialize() {
        toggleGroup = new ToggleGroup();
        YksiButton.setToggleGroup(toggleGroup);
        KolmeButton.setToggleGroup(toggleGroup);
        ViisiButton.setToggleGroup(toggleGroup);

        PelaaButton.setDisable(true);
        toggleGroup.selectedToggleProperty().addListener((_, _, newValue) -> {
            if (newValue != null) {
                PelaaButton.setDisable(false);
            }
        });
    }

    @FXML public void aloitaPeli() {
        if (YksiButton.isSelected()) {
            tarvittavatVoitot = 1;
        } else if (KolmeButton.isSelected()) {
            tarvittavatVoitot = 3;
        } else if (ViisiButton.isSelected()) {
            tarvittavatVoitot = 5;
        }
        System.out.println("Tarvittavat voitot: " + tarvittavatVoitot);
    }
    
    @FXML public void switchToMainView() {
        try {
            aloitaPeli();

            FXMLLoader loader = new FXMLLoader(getClass().getResource("/application/view/MainView.fxml"));
            Parent mainViewRoot = loader.load();

            MainViewController mainController = loader.getController();
            mainController.asetaVoittojenMaara(tarvittavatVoitot);

            Stage stage = (Stage) PelaaButton.getScene().getWindow();
            Scene scene = new Scene(mainViewRoot);

            String css = getClass().getResource("/application/application.css").toExternalForm();

            scene.getStylesheets().add(css);
            
            stage.setScene(scene);
            stage.show();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}