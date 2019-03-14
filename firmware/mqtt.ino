WiFiClientSecure _espClient;
//PubSubClient _mqClient(_espClient);
PubSubClient _mqClient(mqttServer, mqttServerPort, mqttCallback, _espClient);

int _reconnectAttemptCounter = 0;
long _nextReconnectAttempt = 0;

char _commandTopic[70];
char _statusTopic[70];
char _regToken[20];
char _regTopic[70];

//callback when a mqtt message is recieved
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on ");
  Serial.println (topic);
  if (strcmp(topic, _commandTopic) == 0) {
    if ((char)payload[0] == '1') {
      toogleDoor();
    } else if ((char)payload[0] == '0') {
      toogleDoor();
    } else if ((char)payload[0] == '3') {
      sendCurrentDoorStatus();
    }
  }
}

void mqttSetup() {
  _espClient.setInsecure(); 
}

void mqttLoop() {
  if (!_mqClient.connected()) {
    mqttConnect();
  } else {
    _mqClient.loop();
  }
}

void mqttConnect() {
  if (!_mqClient.connected() && _nextReconnectAttempt < millis() ) {
    Serial.println("Connecting to MQTT Server....");
    if (_mqClient.connect(hostname, mqttUsername, mqttPassword)) {

      mqttSubscribe();

      _reconnectAttemptCounter = 0;
      _nextReconnectAttempt = 0;

    } else {
      Serial.print("Failed to connect to ");
      Serial.println(MQTT_SERVER);

      _reconnectAttemptCounter++;
      _nextReconnectAttempt = sq(_reconnectAttemptCounter) * 1000;
      if (_nextReconnectAttempt > 30000) _nextReconnectAttempt = 30000;

      Serial.print("Will reattempt to connect in ");
      Serial.print(_nextReconnectAttempt);
      Serial.println(" seconds");

      _nextReconnectAttempt += millis();
    }
  }
}

void mqttSendMsg(char * msg) {
  if (_mqClient.connected() && (strlen(_statusTopic) > 0)) {
    _mqClient.publish((char *)_statusTopic, msg);
  }
}

void mqttSubscribe() {
  sprintf (_commandTopic, "garage/%s/command", hostname);
  Serial.println(_commandTopic);

  sprintf (_statusTopic, "garage/%s/status", hostname);
  Serial.println(_statusTopic);

  _mqClient.subscribe(_commandTopic);
}

