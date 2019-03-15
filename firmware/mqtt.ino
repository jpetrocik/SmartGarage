WiFiClientSecure _wifiClientSecure;
WiFiClient _wifiClient;
PubSubClient _mqClient;

char _commandTopic[150];
char _statusTopic[150];
int _reconnectAttemptCounter = 0;
long _nextReconnectAttempt = 0;

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
  _wifiClientSecure.setInsecure(); 
  
  if (mqttSsl) {
    Serial.println("Using SSL connection with mqtt");
    _mqClient.setClient(_wifiClientSecure);
  } else {
    _mqClient.setClient(_wifiClient);
  }
  
  _mqClient.setServer(mqttServer, mqttServerPort);
  _mqClient.setCallback(mqttCallback);

  /**
   * Two different variables are used to keep seperate
   * when a custom topic is configed versus using the
   * default topics.
   */
  if (strlen(commandTopic)) {
    strncpy(_commandTopic, commandTopic, 150);
  } else {
    sprintf (_commandTopic, "garage/%s/command", hostname);
  }

  if (strlen(statusTopic)) {
    strncpy(_statusTopic, statusTopic, 150);
  } else {
    sprintf (_statusTopic, "garage/%s/status", hostname);
  }
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
    char clientId[20];
    sprintf (clientId, "garage%08X", ESP.getChipId());

    Serial.print("Connecting to ");
    Serial.print(mqttServer);
    Serial.print(" as ");
    Serial.print(clientId);
    Serial.print("...");

    boolean connectStatus = 0;
    if (strlen(mqttUsername)) {
      connectStatus = _mqClient.connect(clientId, mqttUsername, mqttPassword);
    } else {
      connectStatus = _mqClient.connect(clientId);
    }

    if (connectStatus) {
      Serial.println(" connected!");

      mqttSubscribe();

      _reconnectAttemptCounter = 0;
      _nextReconnectAttempt = 0;

    } else {
      Serial.println(" failed!");
      Serial.println(mqttServer);

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
  _mqClient.subscribe(_commandTopic);
  Serial.print("Subscribed to ");
  Serial.println(_commandTopic);
}

