ESP8266WebServer server(80); 


void webServerSetup() {
  Serial.println("Starting web server on port 80");
   server.on("/", handleStatus); 
   server.on("/door", handleDoor); 
   server.on("/version", HTTP_GET, handleVersion); 
   server.on("/restart", HTTP_POST, handleRestart); 
   server.on("/factoryreset", HTTP_POST, handleFactoryReset); 
   server.on("/config", HTTP_GET, handleConfigureDevice); 
   server.on("/config", HTTP_PUT, handleSaveConfigureDevice); 
   server.on("/config", HTTP_OPTIONS, handleConfigureOptions); 

   server.begin();
}

void webServerLoop() {
  server.handleClient();
}

void handleDoor() {
  toogleDoor();
  server.send(200, "application/json",  (char *)jsonStatusMsg);
}

void handleStatus() {
  server.send(200, "application/json",  (char *)jsonStatusMsg);
}

void handleRestart() {
  server.send(200, "application/json",  "{\"message\":\"Restarting\"}");
  delay(1000);
  ESP.restart();
}

void handleFactoryReset() {
  server.send(200, "application/json",  "{\"message\":\"Factory Reset\"}");
  delay(1000);
  factoryReset();
}

void handleVersion() {
  server.send(200, "application/json",  "{\"version\":\"" __DATE__ " " __TIME__ "\"}");
}

void handleConfigureOptions() {
  server.sendHeader("Access-Control-Allow-Origin", "*", false);
  server.sendHeader("Access-Control-Allow-Methods", "GET,PUT,OPTIONS", false);
  server.send(200, "text/html", "");
}

void handleConfigureDevice() {
  Serial.println("Loading config data....");
  if (SPIFFS.exists("/config.json")) {
    //file exists, reading and loading
    File configFile = SPIFFS.open("/config.json", "r");
    if (configFile) {
      size_t size = configFile.size();
      std::unique_ptr<char[]> buf(new char[size]);

      configFile.readBytes(buf.get(), size);

      DynamicJsonDocument json(1024);
      DeserializationError error = deserializeJson(json, buf.get());

      if (!error) {
        String result;
        serializeJsonPretty(json, result);
        server.sendHeader("Access-Control-Allow-Origin", "*", false);
        server.send(200, "application/json",  result);
        return;
      }
    }
  }
  server.sendHeader("Access-Control-Allow-Origin", "*", false);
  server.send(200, "application/json",  "{}");

}

void handleSaveConfigureDevice() {
  int argCount = server.args();
  for (int i=0; i<argCount; i++){
    String argName = server.argName(i);
    String argValue = server.arg(i);

    if (argName == "hostname") {
      argValue.toCharArray(hostname, 20);
    } else if (argName == "mqttServer") {
      argValue.toCharArray(mqttServer, 150);
    } else if (argName == "mqttUsername") {
      argValue.toCharArray(mqttUsername, 150);
    } else if (argName == "mqttPassword") {
      argValue.toCharArray(mqttPassword, 150);
    } else if (argName == "mqttServerPort") {
      mqttServerPort = argValue.toInt();
    } else if (argName == "commandTopic") {
      argValue.toCharArray(commandTopic, 150);
    } else if (argName == "statusTopic") {
      argValue.toCharArray(statusTopic, 150);
    } 
  }

  configSave();

  handleConfigureDevice();
}

