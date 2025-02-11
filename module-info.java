module KIVIPAPERISAKSET {
	requires javafx.controls;
	requires javafx.fxml;
	 
	exports application;
	exports application.controller;
	
	opens application.controller to javafx.fxml;
}
