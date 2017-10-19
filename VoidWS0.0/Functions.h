/***********************************************/
/*                FILE SYSTEM                  */
/***********************************************/

void formatFS() {
  Serial.println("Formatting SPIFFS");
  SPIFFS.format();
  Serial.println("DONE, U're clean kiddo!");
}

void rebootESP() {
  Serial.println("reboot in 2 sec");
  delay(2000);
  ESP.reset();
  delay(2000);
}

/***********************************************/
/*                    WiFI                     */
/***********************************************/
bool scanWiFi() {
  String findWiFi[30];              //can be a lot better....
  bool networkScanResult = false;
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("Setup done");
  delay(500);
  Serial.println("scan start");
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  }
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      findWiFi[i] = String( WiFi.SSID(i));
      Serial.print("array[");
      Serial.print(i + 1);
      Serial.print("]");
      Serial.println(findWiFi[i]);
      if (findWiFi[i] == c_MyWiFi) {
        networkScanResult = Found_WiFi;
        Serial.println("FOUND WIFI!!!");
        break;
      }
    }
  }
  return networkScanResult;
}

void startAP() {
  Serial.println("Configuring access point...");
  WiFi.softAP(deviceName);
  IPAddress APIP = WiFi.softAPIP();
  myIP = String(APIP[0]) + String(".") + \
         String(APIP[1]) + String(".") + \
         String(APIP[2]) + String(".") + \
         String(APIP[3]);
  Serial.println(myIP);
  Serial.print("AP IP address: ");

  Serial.println(APIP);
  myIP = String(APIP);
  myMAC = WiFi.macAddress();
  APStart = true;
}


void setup_wifi(char ssid[], char pswd[]) {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.hostname(deviceName);
  WiFi.begin(ssid, pswd);
  unsigned long millisAtConnection = millis();
  byte wifiStatus = WiFi.status();

  while (wifiStatus != WL_CONNECTED ) {

    if ((unsigned long)(millis() - millisAtConnection) >= WiFiTimeoutInSeconds * 1000) {
      Serial.println();
      Serial.print("couldn't connect: ");
      switch (wifiStatus) {
        case 0: Serial.println("Wi-Fi is in process of changing between statuses");
          break;
        case 1: Serial.println("configured SSID cannot be reached");
          break;
        // case WL_CONNECTED: Serial.println("actually is't connected just after hitting timeout (this is happening once every 10^1000000 times...)");
        //   break;
        case 4: Serial.println("password is incorrect");
          break;
        case 6: Serial.println("module is not configured in station mode (more likley too short timeout time)");
          break;
        default: Serial.println("REASON UNKNOWN");
          break;
      }
      Serial.println("hit time out... starting AP");
      startAP();
      break;
    }
    wifiStatus = WiFi.status();
    delay(500);
    Serial.print(".");
  }
  if (wifiStatus == WL_CONNECTED) {

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    IPAddress ipAddress = WiFi.localIP();
    myIP = String(ipAddress[0]) + String(".") + \
           String(ipAddress[1]) + String(".") + \
           String(ipAddress[2]) + String(".") + \
           String(ipAddress[3])  ;
    Serial.println(myIP);
    myMAC = WiFi.macAddress();
  }
}

void StartWiFi() {
  if (scanWiFi()) {
    setup_wifi(c_MyWiFi, c_MyPSW);

  } else {
    Serial.println();
    startAP();
  }
}


boolean reconnect();
void nonBlockingMQTTConnection() {
  randomSeed(micros());
  if (String(mqtt_enable) == "1") {
    if (!client.connected()) {
      mqttConnectionStatus = false;
      unsigned long now = millis();
      if (now - lastReconnectAttempt > 5000) {
        lastReconnectAttempt = now;
        if (reconnect()) {
          lastReconnectAttempt = 0;
          mqttConnectionStatus = true;
        }
      }
    }
    else {
      client.loop();
    }
  }
}

void SerialPrintTopicsAndPayload() {
  Serial.println("Recieved a message");
  Serial.print("topic: [");
  Serial.print(topic);
  Serial.println("]");
  Serial.print("payload: [");
  Serial.print(payload);
  Serial.println("]");
}

/***********************************************/
/*      READ/WRITE WiFi PARAMETERS IN FS       */
/***********************************************/

void saveParametersToFS4WiFi() {
  Serial.println("saving config");
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["wifi_ssid"] = c_MyWiFi;
  json["wifi_password"] = c_MyPSW;

  File configFile = SPIFFS.open("/configWiFi.json", "w");
  if (!configFile) {
    Serial.println("failed to open config file for writing");
  }
  json.printTo(Serial);
  json.printTo(configFile);
  configFile.close();
}

void readFS4WiFi() {
  Serial.println("mounting WiFi FS...");
  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/configWiFi.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/configWiFi.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");
          strcpy(c_MyWiFi, json["wifi_ssid"]);
          strcpy(c_MyPSW, json["wifi_password"]);

        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS for WiFi");
  }
}

/***********************************************/
/*      READ/WRITE DEVICE PARAMETERS IN FS     */
/***********************************************/

void saveParametersToFS4Device() {
  Serial.println("saving Device config");
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["device_name"] = deviceName;

  File configFile = SPIFFS.open("/configDevice.json", "w");
  if (!configFile) {
    Serial.println("failed to open config file for writing");
  }
  json.printTo(Serial);
  json.printTo(configFile);
  configFile.close();
}

void readFS4Device() {
  Serial.println("mounting Device FS...");
  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/configDevice.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/configDevice.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");
          strcpy(deviceName, json["device_name"]);

        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS for WiFi");
  }
  //end read
}

/***********************************************/
/*      READ/WRITE MQTT PARAMETERS IN FS       */
/***********************************************/
void readFS4MQTT() {
  Serial.println("mounting MQTT FS...");
  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/configMQTT.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/configMQTT.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");
          strcpy(mqtt_enable, json["mqtt_enable"]);
          strcpy(mqtt_server, json["mqtt_server"]);
          strcpy(mqtt_port, json["mqtt_port"]);
          strcpy(mqtt_username, json["mqtt_username"]);
          strcpy(mqtt_password, json["mqtt_password"]);
          strcpy(header_topic, json["header_topic"]);
        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read
}

void saveParametersToFS4MQTT() {
  Serial.println("saving MQTT config");
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["mqtt_enable"] = mqtt_enable;
  json["mqtt_server"] = mqtt_server;
  json["mqtt_port"] = mqtt_port;
  json["mqtt_username"] = mqtt_username;
  json["mqtt_password"] = mqtt_password;
  json["header_topic"] = header_topic;
  File configFile = SPIFFS.open("/configMQTT.json", "w");
  if (!configFile) {
    Serial.println("failed to open config file for writing");
  }
  json.printTo(Serial);
  json.printTo(configFile);
  configFile.close();
  //end save
}

/***********************************************/
/*                   HANDLERS                  */
/***********************************************/

void handleRoot() {
  server.send(200, "text/html", rootWebPage);
}

void handleWiFiArgs() { //Handler for WiFi credentials
  String message;
  Serial.print("server.args(): ");
  Serial.print(server.args());
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println();
  message = "WiFi Credentials Set: <br>";

  for (int i = 0; i < server.args(); i++) {
    message += server.argName(i) + ": ";     //Get the name of the parameter
    message += server.arg(i) + "</br>";      //Get the value of the parameter
  }

  server.arg(0).toCharArray(c_MyWiFi, 32);
  server.arg(1).toCharArray(c_MyPSW, 64);

  saveParametersToFS4WiFi();

  message += "<br>SAVING YOUR BRAND NEW WiFi Credentials <br>";
  message += "<h1>I'm going to reboot in 2 seconds..<h1>";
  String answer = WebPageInit + message + WebPageClose;
  server.send(200, "text/html", answer);
  rebootESP();
}

void handleDeviceArgs() { //Handler for FiWi cedentials
  String message;
  Serial.print("server.args(): ");
  Serial.print(server.args());
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println();
  message = "Device Name Set: <br>";
  for (int i = 0; i < server.args(); i++) {
    message += server.argName(i) + ": ";     //Get the name of the parameter
    message += server.arg(i) + "</br>";              //Get the value of the parameter
  }
  server.arg(0).toCharArray(deviceName, 32);
  saveParametersToFS4Device();
  message += "<h1>I'm going to reboot in 2 seconds..<h1>";
  String answer = WebPageInit + message + WebPageClose;
  server.send(200, "text/html", answer);
  rebootESP();
}

void handleMQTTArgs() { //Handler for DHT
  String message;
  Serial.print("server.args(): ");
  Serial.print(server.args());
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println();
  if (server.args() == 6) { //Get number of parameters if 4 DHT enable if 3 NOT
    message = "Number of arguments received: <br>";
    for (int i = 0; i < server.args(); i++) {
      /*    [0] enable
            [1] broker
            [2] port
            [3] user
            [4] pass
            [5] top header
      */
      message += "</br>" "Arg nr " + (String)i + " -> ";   //Include the current iteration value
      message += server.argName(i) + ": ";     //Get the name of the parameter
      message += server.arg(i) + "</br>";              //Get the value of the parameter
    }

    mqtt_enable[0] = '1';
    server.arg(1).toCharArray(mqtt_server, 40);
    server.arg(2).toCharArray(mqtt_port, 8);
    server.arg(3).toCharArray(mqtt_username, 32);
    server.arg(4).toCharArray(mqtt_password, 32);
    server.arg(5).toCharArray(header_topic, 34);
    saveParametersToFS4MQTT();
    message += "<br>SAVING YOUR BRAND NEW BROKER PARAMETERS <br>! let your things talk !";
  }
  else {
    mqtt_enable[0] = '0';
    saveParametersToFS4MQTT();
    message = "<br>you disabled MQTT communication </br><h2>nothing ventured nothing gained</h2> rebooting... U won't have any DHT sensor";
  }
  message += "<h1>I'm going to reboot in 2 seconds..<h1>";
  String answer = WebPageInit + message + WebPageClose;
  server.send(200, "text/html", answer);
  rebootESP();
}


//                       HEADER         TOPIC
bool StringComposer(char header[], char actionTopic[]) {
  String stringComposer = String(header) + "/" + String(actionTopic);
  if (stringComposer.length()  >= MAX_TOPIC_LENGTH) {
    Serial.print(" ERROR!! \n length of topic exceed the max value of ");
    Serial.print(MAX_TOPIC_LENGTH - 1);
    Serial.println(" characters. Impossible to create sush a topic");
    Serial.println(actionTopic);
    return false;
  } else {
    stringComposer.toCharArray(actionTopic, stringComposer.length() + 1);
    Serial.println(actionTopic);
    return true;
  }
}
