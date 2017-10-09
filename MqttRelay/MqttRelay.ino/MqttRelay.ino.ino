/*  MQTT switch (relay control)

    - WiFi INITIALISAZION: as described embedded
    - MQTT stndard pubsubclient LIBRARY.
        - reconnect() connect to the broker and SUBSRIBE to topics
        - callback() recieve the messages this client is subscribed to
    - turnON/turnOFF function
    
    payload on = "ON"
    payload off = "OFF"
    
    home assistant config:
    switch:
      platform: mqtt
      name: "my first controlled relay"
      command_topic: "myESP/setPower" 
#or if headerTopic is changed ->  "{headertopic}/setPower"
      state_topic: "myESP/powerSet"
#or if headerTopic is changed ->  "{headertopic}/powerSet"  
      payload_on: "ON"
      payload_off: "OFF"
      optimistic: false
      qos: 1
      retain: true
*/

#include <ESP8266WiFi.h>      //https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi
#include <PubSubClient.h>   //https://github.com/knolleary/pubsubclient


/************ WIFI and MQTT AUTENTICATION ******************/
/****************    CUSTOMIZE THIS    *********************/
/*                       ||||||
                         ||||||
                         ||||||
                        \||||||/
                         \||||/
                          \||/
                           \/                             */

#define wifi_ssid "YOUR_NETWORKNAME"      //your WiFi name
#define wifi_password "YOUR_PASSWORD"     //your WiFi password
#define mqtt_server "YOUR_MQTT_BROKER"    //your MQTT broker ip address or host name
#define mqtt_port 1883                    //your MQTT broker port
#define mqtt_user "super_cool_username"   //your MQTT username (if not set the parameter is ignored by the broker)
#define mqtt_password "SuperSecretPSWD"   //your MQTT password (if not set the parameter is ignored by the broker)
char headerTopic[64] = "myESP";           //this will create a myESP/{topic identifier}

#define relayPin D1  //define pin

/****************************** MQTT TOPICS  ***************************************/
char RelayPowerSub[64] = "setPower";    //  turn ON turn OFF
char RelayPowerPub[64]  = "powerSet";    //  and confirm

WiFiClient espClient;                     //initialise a wifi client
PubSubClient client(espClient);         //creates a partially initialised client instance for MQTT connection
char msg_buff[50];                        //for storing incoming/outcoming messages
String topic;
String payload;
int RelayState = 0;                       //initial state of relay

void setup_wifi() {
  randomSeed(micros());
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Once (re)connected (re)subscribe
      client.subscribe(RelayPowerSub);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* c_topic, byte* c_payload, unsigned int c_length) {

  for (int i = 0; i < c_length; i++) {
    msg_buff[i] = c_payload[i];
  }
  msg_buff[c_length] = '\0';
  String payload = String(msg_buff);
  String topic = String(c_topic);
  Serial.println();
  Serial.println("Message arrived: ");
  Serial.print("topic: [");
  Serial.print(topic);
  Serial.println("]");
  Serial.print("payload: [");
  Serial.print(payload);
  Serial.print("]");

  if (payload == "ON") {
    digitalWrite(relayPin, HIGH);   // Turn the LED on
    client.publish(RelayPowerPub, msg_buff);
  }
  if (payload == "OFF") {
    digitalWrite(relayPin, LOW);   // Turn the LED on
    client.publish(RelayPowerPub, msg_buff);
  }
}
void setup() {
  // put your setup code here, to run once:
  pinMode(relayPin, OUTPUT);
  Serial.begin(115200);
  Serial.println();
  digitalWrite(relayPin, LOW);
  StringComposer(headerTopic, RelayPowerPub);
  Serial.print("Your relay Command topic: ");
  Serial.println(RelayPowerPub);
  StringComposer(headerTopic, RelayPowerSub);
  Serial.print("Your relay Status topic: ");
  Serial.println(RelayPowerSub);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);  //client is now ready for use
  client.setCallback(callback);              //client is now listening for incoming messages
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
//                       HEADER         TOPIC
void StringComposer(char header[], char actionTopic[]){  
  String stringComposer = String(header) + "/"+ String(actionTopic);
  stringComposer.toCharArray(actionTopic, 64);
}

