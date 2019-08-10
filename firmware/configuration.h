#ifndef Configuration_h
#define Configuration_h

//Board version, found on the back of the board
#define VERSION_1_1 1
#define VERSION_1_2 2

//Define the board version
#define BOARD_VERSION VERSION_1_2

//To enable OTA updates
#define OTA_ENABLED

//Defaults to mDns look up of mqtt, change to server name or ip
#define MQTT_SERVER "mqtt.local"
#define MQTT_PORT 1883

//v1.1 PINOUTS
#if BOARD_VERSION == VERSION_1_1
  #define ONBOARD_LED 2
  #define DOOR_STATUS 12
  #define RELAY 14
#endif

//v1.2 PINOUTS
#if BOARD_VERSION == VERSION_1_2
  #define ONBOARD_LED 2
  #define DOOR_STATUS 12
  #define RELAY 14
  #define DOOR_LOCK 13
#endif




#endif
