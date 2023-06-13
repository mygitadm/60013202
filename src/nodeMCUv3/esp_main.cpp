// Include Libraries
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <time.h>
#include <TimeLib.h>
#include <SPI.h>
#include <SD.h>


#include "credentials.h"

// Wi-FI SSID and PASWWORD 
const char *ssid = STASSID;
const char *password = STAPSK;

// Global Variables

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
bool isWebSocketConnected = false;

static unsigned long lastRequestTimeRealtime = 0;
static unsigned long lastRequestTimeHistorical = 0;
static unsigned long lastRequestTime = 0;

const int led = 13;
const unsigned long logInterval = 60 * 1000L; // Log every 60 seconds
bool sdCardPresent = false;
const int chipSelect = 4;
bool fileInUse = false;


// Function Declarations
void initializeWiFi();
void initializeServer();
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
void initializeSDCard();
void logDataToSD(time_t timestamp, float solutionTemp, float substrateTemp, float substrateMoisturePercent, float atmosTemp, float atmosHumidity);
void requestDataFromArduino(String dataType);
//void handleDataRequest(AsyncWebServerRequest *request);
//void handleSetMoisture();



// Here goes the implementation of the above declared functions
void initializeWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}
///======how to detect sd card removal?===///
void logDataToSD(time_t timestamp, float solutionTemp, float substrateTemp, float substrateMoisturePercent, float atmosTemp, float atmosHumidity)
{

  if (!sdCardPresent || fileInUse)
  {
    Serial.println("error - SD card doesn't present or in use");
    return;
  }
  
 

  fileInUse = true;

  // Check if the file exists and create it with headers if it doesn't
  if (!SD.exists("infoLog.csv"))
  {
    File dataFile = SD.open("infoLog.csv", FILE_WRITE);
    if (dataFile)
    {
      dataFile.println("Timestamp,SolutionTemp,SubstrateTemp,AtmosTemp,AtmosHumidity,SubstrateMoisturePercent");
      dataFile.close();
    }
    else
    {
      Serial.println("error creating infoLog.csv");
      fileInUse = false;
      return; // If the file couldn't be created, exit the function
    }
  }

  File dataFile = SD.open("infoLog.csv", FILE_WRITE);

  if (dataFile)
  {
    // Convert timestamp to tm structure
    tmElements_t tm;
    breakTime(timestamp, tm);

    // ISO 8601 timestamp format
    char timestamp_str[25];
    snprintf(timestamp_str, sizeof(timestamp_str), "%04d-%02d-%02dT%02d:%02d",
             tmYearToCalendar(tm.Year), tm.Month, tm.Day, tm.Hour, tm.Minute);

    // CSV format
    dataFile.print(String(timestamp_str) + ",");
    dataFile.print(String(solutionTemp) + ",");
    dataFile.print(String(substrateTemp) + ",");
    dataFile.print(String(atmosTemp) + ",");
    dataFile.print(String(atmosHumidity) + ",");
    dataFile.println(String(substrateMoisturePercent));

    dataFile.close();
    Serial.println("logging has been occurred");
  }
  else
  {
    Serial.println("error opening infoLog.csv");
  }
  fileInUse = false;
}

void requestDataFromArduino(String dataType) {
    // Send the request to the Arduino
    Serial.println("{\"request\": \"" + dataType + "\"}");
    Serial1.println("{\"request\": \"" + dataType + "\"}");

    // Wait for the response
    unsigned long startTime = millis();
    while (!Serial.available()) {
        if (millis() - startTime > 2000) { // Timeout after 2 seconds
            Serial.println("Timeout waiting for response from Arduino");
            return;
        }
        delay(100);
    }
    
    // Read the response
    String data = "";
    while(Serial.available()){
        char c = Serial.read();
        if(c == '\n')
            break;
        data += c;
    }
    
    Serial.println("Received from Arduino: " + data);
    
    // Parse JSON
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, data);
    if (error) {
        Serial.println(F("Failed to parse JSON"));
        return;
    }

    const char* receivedDataType = doc["dataType"];
    if (strcmp(receivedDataType, "realtime") == 0) {
        // Send data to all connected clients
        ws.textAll(data);
    } else if (strcmp(receivedDataType, "historical") == 0) {
        // Save data to SD card
        if (!fileInUse) {
            logDataToSD(doc["timestamp"], doc["solutionTemp"], doc["substrateTemp"], doc["substrateMoisture"], doc["atmosTemp"], doc["atmosHumidity"]);
            
        }
    } else {
        Serial.println("Unknown data type received");
    }
}



void initializeServer() {
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html");
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/style.css", "text/css");
  });

  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/script.js", "text/javascript");
  });

  //server.on("/setMoisture", HTTP_POST, [](AsyncWebServerRequest *request){
    // handleSetMoisture
  //});

  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(404, "text/plain", "File not found");
  });

  //server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    //handleDataRequest(request);
  //});

  server.begin();
  Serial.println("HTTP server started");
}

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        isWebSocketConnected = true;
    } else if (type == WS_EVT_DISCONNECT) {
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        isWebSocketConnected = false;
    }
}

void initializeSDCard()
{
  if (!SD.begin(chipSelect))
  {
    Serial.println("Card failed, or not present");
    sdCardPresent = false;
  }
  else
  {
    Serial.println("Card initialized.");
    sdCardPresent = true;
    if (!SD.exists("infoLog.csv"))
    { // Check if file already exists
      File dataFile = SD.open("infoLog.csv", FILE_WRITE);
      if (dataFile)
      {
        dataFile.println("timestamp,solutionTemp,substrateTemp,atmosTemp,atmosHumidity,substrateMoisturePercent"); // Write the headers
        dataFile.close();
      }
    }
  }
}

// Setup and Loop
void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  Serial1.begin(115200);
  delay(1000);
  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  
  initializeWiFi();
  initializeServer();
  initializeSDCard();
  lastRequestTime = millis();
}

void loop(void) {
  if (isWebSocketConnected && millis() - lastRequestTimeRealtime >= 2000) {
      requestDataFromArduino("realtime");
      lastRequestTimeRealtime = millis();
  }
  // Request logging data from Arduino every 60 seconds
  if (millis() - lastRequestTimeHistorical >= logInterval) {
      requestDataFromArduino("historical");
      lastRequestTimeHistorical = millis();
  }
  MDNS.update();
}
