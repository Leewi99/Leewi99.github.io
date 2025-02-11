package application.controller;

import java.io.IOException;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
import javafx.stage.Window;

public class GameOverViewController {

    @FXML private Label TulosLabel;
    @FXML private Button UusiPeliButton;

    public void pelinTulos(String tulos) {
        TulosLabel.setText(tulos);
    }

    @FXML public void aloitaUusiPeli() {
        try {
            FXMLLoader loader = new FXMLLoader(getClass().getResource("/application/view/GameView.fxml"));
            Parent gameViewRoot = loader.load();

            Stage stage = (Stage) UusiPeliButton.getScene().getWindow();
            Scene scene = new Scene(gameViewRoot);
            
            String css = getClass().getResource("/application/application.css").toExternalForm();
            
            scene.getStylesheets().add(css);
            stage.setScene(scene);
            stage.show();

            for (Window window : Stage.getWindows()) {
                if (window instanceof Stage && window != stage) {
                    ((Stage) window).close();
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}