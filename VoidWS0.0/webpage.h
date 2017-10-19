
void composeWebPage() {

  WebPageInit = "<!DOCTYPE html> <html> <head> <style>"
                "header { background-color:#00293C; color: #EAAF00; padding:10; min-height: 40px; font-size:20px; text-align: center; }"
                "body{ margin:0; padding:0; color: #00293C; font-family:sans-serif; font-variant: small-caps; font-size:100%; text-align: center; }"
                "footer { background-color:#EAAF00; color: #206871; padding:10; min-height: 40px; font-size:15px; text-align: right; } div{ padding: 4% 10%; }"
                "    .one{ background-color: #CFD8DC; background-image: -webkit-linear-gradient(30deg, #CFD8DC 50%, #B0BEC5 50%); min-height: 500px; }"
                "    .formbox{ } a:link, a:visited { background-color: #EAAF00; border: none; color: #206871; padding: 5px; text-align: center; text-decoration: none; display: inline-block; font-size: 20px; font-variant: small-caps; font-weight: bold; margin: 4px 2px; cursor: pointer; border-radius: 4px; } a:hover, a:active { background-color: #DE2500; color: #CECECE; }"
                /*this below is only for forms*/
                "    .descriptionBox{ border-radius: 5px; background-color: #f2f2f2; padding: 20px; } input[type=text],"
                "        select { width: 50%; padding: 12px 20px; margin: 8px 0; display: inline-block;"
                "        border: 1px solid #ccc; border-radius: 4px; box-sizing: border-box; }"
                "    input[type=submit] { width: 50%; background-color: #00293C; color: white; padding: 14px 20px; margin: 8px 0; border: none; border-radius: 4px; cursor: pointer; font-family:sans-serif; font-variant: small-caps; font-size:150%; }"
                "    input[type=submit]:hover { background-color: #EAAF00; } </style> </head> <body> <header>";

  WebPageInit +=  String(deviceName) + "</header><div class=\"one\">";
  //then start  with the body ( header declaration with device name on top included above) and finish with WebPageClose
  // HERE: opens the body and <div class one>

  WebPageClose = "</div><footer><br>powerd by M1k3.Pap4</br>GNU licence -> FREE as in FREEDOM</footer></body> </html>";

  String SubmitButton = "<input type=\"submit\" value=\"!tell this to your micro!\"> </form> (and wait for saving config and rebooting)";

  String rootWebPageBODY = "<a href=/wifi>WiFi</a></p>"
                           "<p><a href=/mqtt>MQTT</a></p>"
                           "<p><a href=/device>Active Settings</a></p>"
                           "<h1 class=\"center\">" + Title + "</h1> <p>" + Description + "</p> ";

  String WiFiPageBODY = "<div class=\"descriptionBox\"> <h1>WiFi config</h1><form action=\"/WiFiparameters\">"
                        "<p><label for=\"WiFiSSID\">Network Name</label> </p><p><input type=\"text\" id=\"WiFiSSID\" name=\"WiFi.SSID\" placeholder=\"Name of your network\"></p>"
                        "<p><label for=\"WiFiPSW\">Network Password</label> </p><p><input type=\"text\" id=\"WiFiPSW\" name=\"WiFi.Password\" placeholder=\"Super secret network password\"></p>"
                        + SubmitButton + "</div>";

  
  String DevicePageBODY = "<div class=\"descriptionBox\">"
                          "<p>IP address: </p>" + myIP +
                          "<p>MAC address: </p>" + myMAC +
                          "<form action=\"/Deviceparameters\">"
                          "<p><label for=\"DeviceName\">Device and ACCESS POINT Name</label></p><p> <input type=\"text\" id=\"DeviceName\" name=\"Device.Parameters\" value=\""
                          + String(deviceName) +
                          "\" placeholder=\"devicename\"></p>"
                          "<p>" + SubmitButton + "</p>"
                          "<p><a href=/reboot>REBOOT DEVICE</a></p>"
                          "<p><a href=/fsFormat>ERASE ALL DATA</a></p>"
                          "</div>";

  String MqttPageBODY = "<div class=\"descriptionBox\"> <h1>MQTT config</h1><form action=\"/MQTTparameters\">"
                        "<p><input type=\"checkbox\" name=\"MQTTeneble\" value=\"MQTTenable\"/>enable MQTT communication</p>"
                        "<p><label for=\"MQbroker\">MQTT broker</label> <input type=\"text\" id=\"MQTTbroker\" name=\"MQTTbroker\" value = \"" + String(mqtt_server) + "\" placeholder=\"IP address of your MQTT broker\"></p>"
                        "<p><label for=\"MQport\">MQTT port</label> <input type=\"text\" id=\"MQTTport\" name=\"MQTTport\" value = \"" + String(mqtt_port) + "\"placeholder=\"communication port (default :1883)\"></p>"
                        "<p><label for=\"MQuser\">MQTT username</label> <input type=\"text\" id=\"MQTTuser\" name=\"MQTTUsername\" value = \"" + String(mqtt_username) + "\"placeholder=\"your super cool username\"></p>"
                        "<p><label for=\"MQpass\">MQTT password</label> <input type=\"text\" id=\"MQTTuser\" name=\"MQTTPassword\" value = \"" + String(mqtt_password) + "\"placeholder=\"your super secret password\"></p>"
                        "<p><label for=\"MQheader\">MQTT topic header</label> <input type=\"text\" id=\"MQTTuser\" name=\"MQTTtopicHeader\" value = \"" + String(header_topic) + "\"placeholder=\"part of the topic you want to customize (no / at the end )\"></p>"
                        + SubmitButton + "</div>";

  rootWebPage = WebPageInit;
  rootWebPage += rootWebPageBODY;
  rootWebPage += WebPageClose;

  WiFiPage = WebPageInit;
  WiFiPage += WiFiPageBODY;
  WiFiPage += WebPageClose;

  MqttPage = WebPageInit;
  MqttPage += MqttPageBODY;
  MqttPage += WebPageClose;;

  DevicePage = WebPageInit;
  DevicePage += DevicePageBODY;
  DevicePage += WebPageClose;
}

void startWebServer() {
  server.on("/", handleRoot);

  server.on("/wifi", []() {
    server.send(200, "text/html", WiFiPage);
  });

  server.on("/mqtt", []() {
    server.send(200, "text/html", MqttPage);
  });

  server.on("/device", []() {
    server.send(200, "text/html", DevicePage);
  });

  server.on("/WiFiparameters", handleWiFiArgs);
  server.on("/MQTTparameters", handleMQTTArgs);
  server.on("/Deviceparameters", handleDeviceArgs);

  server.on("/reboot", []() {
    server.send(200, "text/plain", "rebooting in 3 seconds...");
    Serial.println("3 sec");
    delay(3000);
    ESP.reset();
    delay(3000);
  });
  server.begin();
  Serial.println("SERVER BEGIN!!");
}
