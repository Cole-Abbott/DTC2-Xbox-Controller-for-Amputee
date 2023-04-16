#include <Arduino.h>
#include <WiFi.h> //to connect to wifi
#include <ESPAsyncWebServer.h> //to create web server
#include <AsyncTCP.h> 
#include "SPIFFS.h" //to read files from filesystem
#include "freertos/FreeRTOS.h"

#define ssid "Device-Northwestern"

// Function prototypes
void initWiFi();
void notifyClients(String data);
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len);
void initWebSocket();
String processor(const String& var);
void readADC(void* parameters);


bool ledState = 0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT); // LED output
  digitalWrite(LED_BUILTIN, LOW);

  //init SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  //connect to wifi
  initWiFi();


  // init server
  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Start server
  server.begin();
  Serial.println("Server Ready");

  //start ADC task
  //xTaskCreatePinnedToCore(readADC, "readADC", 10000, NULL, 1, NULL, 1);
}

void loop() {
  ws.cleanupClients();
  digitalWrite(LED_BUILTIN, ledState);
}

void initWiFi() {
  WiFi.mode(WIFI_STA); 
  Serial.println(WiFi.macAddress());
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println("WiFi connected");
}

void notifyClients(String data) {
  ws.textAll(data);
  Serial.println(data);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  static char message[200];
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "toggle") == 0) {
      ledState = !ledState;
      sprintf(message, "{\"event\": \"toggle\", \"data\": %d}", ledState);
      notifyClients(String(message));
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if (ledState){
      return "ON";
    }
    else{
      return "OFF";
    }
  }
  return String();
}

void readADC(void* parameters) {
  while(1) {
    static char message[200];
    static int adcVal = 0, oldAdcVal = 0;
    adcVal = analogRead(36);

    sprintf(message, "{\"event\": \"adc\", \"data\": %d}", adcVal);
    notifyClients(String(message));
    oldAdcVal = adcVal;
    //freertos delay 200ms
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}