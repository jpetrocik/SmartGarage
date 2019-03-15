# SmartGarage

SmartGarage is a WIFI enabled device that works with most garage doors to check whether the door is open or close and to operate the garage door using a mobile phone or as part of a home automation system.  SmartGarage has a HTTP REST api and supports MQTT to work with existing home automation system, like OpenHab2 and home-assistant.io.  No 3rd party servers are required for SmartGarage to work, you can directly operate it via a HTTP RestAPI or directly intergrate it with any home automation system, like OpenHab2 and home-assistant.io.

The device is based around the ESP8266 Wifi modules and can be easy modifed to fit any custom needs. The are hundreds of tutorials about programming and flashing the ESP8266 module using ArRduinoIDE. The PCB design and firmware is opensource and can be easily customized.


## Device ##

The physical PCB is about 20mm x 40mm and is powered by mini usb cable.  Installation is striaght forward, and does not interfer with any the current operations of the garage door.  All remotes still work along with the manual opener inside your garage.

There are a couple options to get your hands on your own board.  You can order it directly from me on ebay.com with the lastest firmware already flashed and ready to install.  Or you can download the Eagle files from https://github.com/jpetrocik/SmartGarage and build your own.

## Controlling The Garage Door ##

The device has a both a REST api and works with MQTT.

REST Api

```
GET  http://garage.local/
Returns the current status of the door, e.g. open or closed

GET  http://garage.local/door
Operates the door, either closing it or opening it depending on what the current status is.

GET http://garage.local/config
Returns the current configuration

PUT http://garage.local/config?poperty_name=property_value
Sets the configuration property to the provided value

GET http://garage.local/restart
Reboot the device

POST http://garage.local/factoryreset
Resets the device to factory settings
```

MQTT

MQTT is a common messaging protocol used for IoT devices.  Most home automation systems like OpenHab and home-assistant.io  support mqtt messaging. If you are unfamilar with MQTT, check out https://mosquitto.org/ to learn more.

The device listens on the command topic and published the status of the door on the status topic.  The command topic is used to send commands to the device to operate the garage door and checks its status. The default hostname for the device is __garage__, so below replace [hostname] with the hostname of the device.  If you have not changed it, it will be __garage__. 

```
garage/[hostname]/command
```

The messages published to the command topic to operate the door are very simple, just a numeric numbers.  Since a garage door just opens or closes when the garage door opener button is pushed there is no direct open command or close command.  If the door is closed, it will open and if its open it will close. For better support with home automation systems the common commands like 0 for OFF, 1 for ON, and 2 for TOGGLE all trigger the garage door.

```
0 Operate the door 
1 Operate the door 
2 Operate the door 
3 Request current status
```

If you are using mosquito the command line to operate the garage door would like like this:

```
mosquito_pub -t garage/garage/command -m 0
```

When the door opens or closes, the status is published on the status topic.  The message is a json message indicating the current status.  Again, if using mosquito the command line to listen for the status change would be

```
mosquito_sub -t garage/garage/status 
```

Here is an example of the message published with the door is closed.

```
{
  "status": "CLOSED"
}
```



