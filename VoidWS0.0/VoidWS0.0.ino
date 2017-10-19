#include <FS.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>       //https://github.com/knolleary/pubsubclient

#include "init.h"
#include "Functions.h"
#include "webpage.h"


/*** MQTT TOPICS  ***/
char GeneralSub[64] = "espSubscribe";    //  turn ON turn OFF
char GeneralPub[64]  = "espPublish";    //  and confirm

void setup() {
  Serial.begin(115200);
  //formatFS();
  readFS4WiFi();
  readFS4Device();
  StartWiFi();
  if (APStart == false) readFS4MQTT();
  if (String(mqtt_enable) == "1") {
    client.setServer(mqtt_server, atoi(mqtt_port));  //client is now ready for use
    client.setCallback(callback);
    Serial.println("\nMQTT connection settings START!!");
    Serial.println("Preparing Topics...");
    // add your topic to this function to add the topic header
    // as customized from the web portal
    StringComposer(header_topic, GeneralSub);
    StringComposer(header_topic, GeneralPub);
  }
  composeWebPage();
  startWebServer();
  //add your setup code here
}

void loop() {
  nonBlockingMQTTConnection();
  server.handleClient();
  //add your code here
}

void callback(char* p_topic, byte* p_payload, unsigned int p_length) {
  for (int i = 0; i < p_length; i++) {
    msg_buff[i] = p_payload[i];
  }
  msg_buff[p_length] = '\0';
  payload = String(msg_buff);
  topic = String(p_topic);
  SerialPrintTopicsAndPayload();
  //here you recieve the topics and payloads the microcontroller is subscribed to  
}

boolean reconnect() {
  Serial.println("Attempting MQTT connection...");
  String clientId = String(deviceName);
  clientId += String(random(0xffff), HEX);
  if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
    Serial.println("\n CONGRATS!!! U'r CONNECTED TO MQTT BROKER! \n start making your things talk!!!");
    client.subscribe(GeneralSub);
    client.loop();
  }
  return client.connected();
}


