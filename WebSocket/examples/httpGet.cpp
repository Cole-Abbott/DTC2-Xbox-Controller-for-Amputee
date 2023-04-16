#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define ssid "Device-Northwestern"

void initWiFi();


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.macAddress());
  initWiFi();


  String serverPath = "https://www.thecolorapi.com/id?format=json&named=false&hex=13120E";

  HTTPClient http;
  // Your Domain name with URL path or IP address with path
  http.begin(serverPath.c_str());

    // Send HTTP GET request
  int httpResponseCode = http.GET();
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();


}

void loop() {
  // put your main code here, to run repeatedly:
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