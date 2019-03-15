#include <FS.h>

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <Ticker.h>
#include <ArduinoJson.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>

#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Bounce2.h>

#include "configuration.h"

Ticker ticker;

Bounce doorSwitch = Bounce();

char jsonStatusMsg[140];

//configuration properties
char mqttServer[150] = MQTT_SERVER;
int mqttServerPort = MQTT_PORT;
boolean mqttSsl = false;
char mqttUsername[150];
char mqttPassword[150];

char hostname[20];
char commandTopic[150];
char statusTopic[150];

void setup() {
  Serial.begin(115200);

  pinMode(ONBOARD_LED, OUTPUT);
  pinMode(RELAY, OUTPUT);

  //setup pin read with bounce protection and door bouncing when it opens
  doorSwitch.attach(DOOR_PIN, INPUT); 
  doorSwitch.interval(500);

//  sprintf (hostname, "garage_%08X", ESP.getChipId());
  sprintf (hostname, "garage");
  
  //slow ticker when starting up
  //switch to fast tick when in AP mode
  ticker.attach(0.6, tick);

  configLoad();

  wifiSetup();

  mdnsSetup();

#ifdef OTA_ENABLED
  otaSetup();
#endif  

  mqttSetup();

  webServerSetup();

  ticker.detach();

  Serial.println("SmartGarage Firmware");
  Serial.println(__DATE__ " " __TIME__);

}

void loop() {

  mqttLoop();

#ifdef OTA_ENABLED
  otaLoop();
#endif  

  webServerLoop();

  sendDoorStatusOnChange();
}

void toogleDoor() {
  digitalWrite(RELAY, HIGH);
  delay(500);
  digitalWrite(RELAY, LOW);
}

void sendCurrentDoorStatus() {
  int doorState = !doorSwitch.read();
  sprintf (jsonStatusMsg, "{\"status\":%s}", doorState ? "\"OFF\"" : "\"ON\"");

  mqttSendMsg(jsonStatusMsg);
}

void sendDoorStatusOnChange() {
  boolean changed = doorSwitch.update();

  //if the button state has changed, record when and current state
  if (changed) {
    sendCurrentDoorStatus();
    int doorState = !doorSwitch.read();
    digitalWrite(ONBOARD_LED, doorState);
  }

}

void factoryReset() {
  Serial.println("Restoring Factory Setting....");
  WiFi.disconnect();
  SPIFFS.format();
  ESP.eraseConfig();
  Serial.println("Restarting....");
  delay(500);
  ESP.restart();
}

void tick() {
  int state = digitalRead(ONBOARD_LED);
  digitalWrite(ONBOARD_LED, !state);
}

//Called to save the configuration data after
//the device goes into AP mode for configuration
void configSave() {
  DynamicJsonDocument jsonDoc(1024);
  JsonObject json = jsonDoc.to<JsonObject>();

  //standard properties, allways shown when view config
  json["hostname"] = hostname;
  json["mqttServer"] = mqttServer;
  json["mqttServerPort"] = mqttServerPort;
  json["mqttSsl"] = mqttSsl;
  json["mqttUsername"] = mqttUsername;
  json["mqttPassword"] = mqttPassword;

  //advanced properties, only show in config if set
  if (strlen(commandTopic))
    json["commandTopic"] = commandTopic;
  if (strlen(statusTopic))
    json["statusTopic"] = statusTopic;

  File configFile = SPIFFS.open("/config.json", "w");
  if (configFile) {
    Serial.println("Saving config data....");
    serializeJson(json, Serial);
    Serial.println();
    serializeJson(json, configFile);
    configFile.close();
  }
}

//Loads the configuration data on start up
void configLoad() {
  Serial.println("Loading config data....");
  if (SPIFFS.begin()) {
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        size_t size = configFile.size();
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);

        DynamicJsonDocument jsonDoc(size);
        DeserializationError error = deserializeJson(jsonDoc, buf.get());
        serializeJsonPretty(jsonDoc, Serial);

        JsonObject json = jsonDoc.as<JsonObject>();
        if (json.containsKey("hostname")) {
          strncpy(hostname, json["hostname"], 20);
        }

        if (json.containsKey("mqttServer")) {
          strncpy(mqttServer, json["mqttServer"], 150);
        }

        if (json.containsKey("mqttUsername")) {
          strncpy(mqttUsername, json["mqttUsername"], 150);
        }

        if (json.containsKey("mqttPassword")) {
          strncpy(mqttPassword, json["mqttPassword"], 150);
        }

        if (json.containsKey("mqttServerPort")) {
          mqttServerPort = json["mqttServerPort"];
        }

        if (json.containsKey("mqttSsl")) {
          mqttSsl = json["mqttSsl"];
        }

        if (json.containsKey("commandTopic")) {
          strncpy(commandTopic, json["commandTopic"], 150);
        }

        if (json.containsKey("statusTopic")) {
          strncpy(statusTopic, json["statusTopic"], 150);
        }

      }
    }
  }
}

