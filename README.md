# SmartGarage

SmartGarage is a WIFI enabled device that works with most garage door openers. Along with opening and closing the garage door, it provides the current status of the door, whether its open or closed.  The stock firmware provides both a HTTP Restful api and MQTT based interface. No 3rd party servers are required for SmartGarage to work, you can directly operate it via a HTTP RestAPI or MQTT.  It easly intergrates with HomeAssistant, https://www.home-assistant.io/ and other home automation software.

## Controlling The Garage Door ##

RESTful Api

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

MQTT is a common messaging protocol used for IoT devices.  Most home automation systems like OpenHab and home-assistant.io  support mqtt messaging. If you are unfamiliar with MQTT, check out https://mosquitto.org/ to learn more.

The device listens on the command topic and publishes the status of the door on the status topic.  The command topic is used to send commands to the device to operate the garage door. The default hostname for the device is __garage__, below replace [hostname] with the hostname of the device.  If you have not changed it, it will be __garage__. 

```
garage/[hostname]/command
```

The messages published to the command topic are very simple,  just numeric numbers.  When the garage door opener button is pressed the garage door either opens or closes depending on the door's current state.  There is no direct open command or close command, if the door is closed, it will open and if its open it will close.  To better support home automation system the common commands like 0 for OFF, 1 for ON, and 2 for TOGGLE all trigger the garage door.

```
0 Close the door 
1 Open the door 
2 Toggle the door, open or close depending on current state
3 Request current status
```

If you are using mosquito the command line to open the garage door would look like this:

```
mosquito_pub -t garage/garage/command -m 1
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

## Installation Guide ##

https://docs.google.com/document/d/12LUHNYjrVlYSZdDVwD1QsqcnY-EzrYETPhFzhXXMPhE/edit?usp=sharing

## Device ##

The physical PCB is about 20mm x 40mm and is powered by a mini usb cable.  Installation is straight forward and does not interfere the current operation of the garage door, all remotes and openers still work.

There are a couple options to get your hands on your own board.  You can order it directly from me on ebay.com with the latest firmware already flashed and ready to install, or you can download the Eagle files from https://github.com/jpetrocik/SmartGarage and build your own.

![Image Of Device](http://petrocik.net/~john/SmartGarage.jpg)

