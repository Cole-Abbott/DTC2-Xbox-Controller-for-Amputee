#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#define ssid "Device-Northwestern"

void initWiFi();
void handleRoot();

WebServer server(80);

// HTML & CSS contents which display on web server
String HTML = "<!DOCTYPE html>\
<html>\
<body>\
<h1>My First Web Server with ESP32 - Station Mode &#128522;</h1>\
</body>\
</html>";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.macAddress());
  initWiFi();
  Serial.println("WiFi connected");

  // Start web server
  server.on("/", handleRoot);

  server.begin();
  Serial.println("HTTP server started");
  
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}


void handleRoot() {
  server.send(200, "text/html", HTML);
}