# MQTT-devices-for-ESP8266

A variety of sensors and actuators connected to the famous ESP8266 chip (tested with NODEMCU and WEMOS Mini) and communicating via MQTT. Initally developed for speaking with home automation framework like Home Assistant, Domoticz, OpenHab, but suitable for communicating with a generic MQTT platform.

### Actuators TOPIC and PAYLOAD naming criteria
With ACTUATORS we consider all the devices that perform action FROM the microcontroller to the real world (OUTPUTs). Actuators (LEDs,  relays, buzzer, etc...) communicate with Home Assistant standard which means:

- "ON" payload for turn on device
- "OFF" payload for turn off
- Brightness (where supported) the brightness of a light nedds to recieve a value between 0 and 255
- RGB lights (where supported) the color of a lamp is expressed in ad RGB format like following R,G,B

 ACKNOLEDGMENT TOPIC:
 
 Every time a state of an actuator changes (via MQTT or via phisical control) the MCU notifies it communicating the very same payload he recieved (if it recieved that from an MQTT communication). If you want to use it in Home assistant you should use "optimistic: FALSE".
 the criteria is the following:
 - {topic header}/brightnessSet -> command topic
 - {topic header}/setBrightness -> status topic (confirm, or akcnoledgment topic)

## Authors

* **Mike Papa** - *Initial work* - [GitHub](https://github.com/papartmike)


## License

Copyright (C) 2016-2017 by Marco Provolo 

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 You should have received a copy of the GNU General Public License along with this program. If not, see http://www.gnu.org/licenses/.


