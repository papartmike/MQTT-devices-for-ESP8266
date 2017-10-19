#define MAX_TOPIC_LENGTH 64
#define Found_WiFi true
#define No_Matching_WiFi false
#define WiFiTimeoutInSeconds 30
char deviceName[32] = "Brand New Device";
char c_MyWiFi[32];
char c_MyPSW[64];
bool APStart = false;

ESP8266WebServer server(80);
WiFiClient espClient;              //initialise a wifi client
PubSubClient client(espClient);   //creates a partially initialised client instance for MQTT

bool mqttConnectionStatus = false;
char mqtt_enable[2] = "0";
char mqtt_server[40] = "192.168.1.2";
char mqtt_port[8] = "1883";
char mqtt_username[32] = "";
char mqtt_password[32] = "";
char header_topic[34] = "myESP";

unsigned long lastReconnectAttempt = 0;  //for non blocking MQTT connection
String topic;
String payload;
char msg_buff[100]; // to store the message payload format for publishing


/******* WEB PAGES *******/
String WebPageInit;
String WebPageClose;
String rootWebPage;
String WiFiPage;
String MqttPage;
String DevicePage;
 
String Title="A simple web Server";
String Description="AP + STA mode and configuration for ESP8266";

String myIP;
String myMAC;

