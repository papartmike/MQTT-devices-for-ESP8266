# MQTT Relay:

![fritzing relay](papartmike/MQTT-devices-for-ESP8266/MqttRelay/wemos%20relay.png)


## Home assitant integration

if headerTopic is changed ->  "{headertopic}/setPower"
if headerTopic is changed ->  "{headertopic}/powerSet" 

### if you want a switch
if you control a socket or a generic load
switch:
  platform: mqtt
  name: "my first controlled relay"
  command_topic: "myESP/setPower" 
  state_topic: "myESP/powerSet" 
  payload_on: "ON"
  payload_off: "OFF"
  optimistic: false
  qos: 1
  retain: true

### if you want a light
if this relay controls a light then:

light:
  platform: mqtt
  name: "my first controlled relay"
  command_topic: "myESP/setPower" 
  state_topic: "myESP/powerSet" 
  payload_on: "ON"
  payload_off: "OFF"
  optimistic: false
  qos: 1
  retain: true
  
  ## NODE RED integration
  
  
