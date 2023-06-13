// Libraries
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>
#include <Adafruit_BME280.h>
#include <SPI.h>
#include <SD.h>

#include "display.h"
#include "datalog.h"
#include "RTC.h"
#include "sensors.h"
#include "settings.h"

// Define display refresh rate
unsigned long lastDisplayUpdate = 0;
const unsigned long displayRefreshInterval = 250;

// tmElements_t tm;
const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};



//========Data logging to SD card=======//
static unsigned long lastLogTime = 0;
const unsigned long logInterval = 60 * 1000L; // Log every 60 seconds

bool sdCardPresent = false;
const int chipSelect = 4;
//csv file
File infoLog;
//file use flag
bool fileInUse = false;

//=========L293D=========//
unsigned long lastPumpOnTime = 0; // 
const unsigned long pumpOnInterval = 60000; //
const int MOTOR_SPEED_PIN = 44;
const int MOTOR_CONTROL_PIN1 = 43;
const int MOTOR_CONTROL_PIN2 = 42;



//=========relay=======//

//relay
#define relayPin 48

// Function prototypes
void setupSensors();
void setupRTC();
bool getTime(const char *str);
bool getDate(const char *str);
void getBME280Data(float &atmosTemp, float &atmosHumidity);
void getDS18B20Data(float &substrateTemp, float &solutionTemp);
void getSubstrateMoisture(float &substrateMoisture);
void initializeSDCard();
void formatSensorData(float value, char *buffer, const char* unit);
void getDateTime();
void transmitData(time_t timestamp, float solutionTemp, float substrateTemp, float substrateMoisturePercent, float atmosTemp, float atmosHumidity);
void transmitDataHistorical(time_t timestamp, float solutionTemp, float substrateTemp, float substrateMoisturePercent, float atmosTemp, float atmosHumidity);
void logDataToSD(time_t timestamp, float solutionTemp, float substrateTemp, float substrateMoisturePercent, float atmosTemp, float atmosHumidity);
void setupPump();
void setupDisplay();
void draw();

void setup()
{
  Serial.begin(9600);
  Serial1.begin(115200);
  setupRTC();
  setupSensors();
  pinMode(MOISTURE_SENSOR, INPUT);
  initializeSDCard();
  setupPump();
  pinMode(relayPin, OUTPUT);
  //u8g2.begin();
  setupDisplay();
}

void setupPump() {
  // Initialize motor control pins
  pinMode(MOTOR_SPEED_PIN, OUTPUT);
  pinMode(MOTOR_CONTROL_PIN1, OUTPUT);
  pinMode(MOTOR_CONTROL_PIN2, OUTPUT);
  
  // Set default speed to 100%
  analogWrite(MOTOR_SPEED_PIN, 255);
}

void transmitData(time_t timestamp, float solutionTemp, float substrateTemp, float substrateMoisturePercent, float atmosTemp, float atmosHumidity)
{
  //time_t timestamp = now();

  // Create JSON document
  StaticJsonDocument<200> doc;

  // Add values to JSON document
  doc["dataType"] = "realtime";
  doc["timestamp"] = timestamp;
  doc["solutionTemp"] = solutionTemp;
  doc["substrateTemp"] = substrateTemp;
  doc["atmosTemp"] = atmosTemp;
  doc["atmosHumidity"] = atmosHumidity;
  doc["substrateMoisture"] = substrateMoisturePercent;

  // Transmit JSON via Serial to ESP8266
  serializeJson(doc, Serial1);
  Serial1.println();

  // Print the same JSON to Serial for debugging
  serializeJson(doc, Serial);
  Serial.println();
}

void transmitDataHistorical(time_t timestamp, float solutionTemp, float substrateTemp, float substrateMoisturePercent, float atmosTemp, float atmosHumidity)
{
  //time_t timestamp = now();

  // Create JSON document
  StaticJsonDocument<200> doc;

  // Add values to JSON document
  doc["dataType"] = "historical";
  doc["timestamp"] = timestamp;
  doc["solutionTemp"] = solutionTemp;
  doc["substrateTemp"] = substrateTemp;
  doc["atmosTemp"] = atmosTemp;
  doc["atmosHumidity"] = atmosHumidity;
  doc["substrateMoisture"] = substrateMoisturePercent;

  // Transmit JSON via Serial to ESP8266
  serializeJson(doc, Serial1);
  Serial1.println();

  // Print the same JSON to Serial for debugging
  serializeJson(doc, Serial);
  Serial.println();
}

void updateWatering(float atmosTemp, float substrateMoisture) {
  float targetHumidity = getTargetHumidity(atmosTemp);
  if (substrateMoisture < targetHumidity) {
    digitalWrite(relayPin, HIGH);
  } else {
    digitalWrite(relayPin, LOW);
  }
}

void transmitConfig() {
  // Create JSON document
  StaticJsonDocument<200> doc;

  // Add values to JSON document
  doc["dataType"] = "config";
  doc["minTemp"] = minTemp;
  doc["maxTemp"] = maxTemp;
  doc["minHumidity"] = minHumidity;
  doc["maxHumidity"] = maxHumidity;

  // Transmit JSON via Serial1 to ESP8266
  serializeJson(doc, Serial1);
  Serial1.println();

  // Print the same JSON to Serial for debugging
  serializeJson(doc, Serial);
  Serial.println();
}
  

void loop()
{
  time_t timestamp = now();
  unsigned long currentTime = millis();
  getDateTime();
  float atmosTemp, atmosHumidity; 
  getBME280Data(atmosTemp, atmosHumidity);
  float substrateTemp, solutionTemp; 
  getDS18B20Data(substrateTemp, solutionTemp);
  float substrateMoisture;
  getSubstrateMoisture(substrateMoisture);

  float targetMoisture = getTargetHumidity(atmosTemp);
  formatSensorData(targetMoisture, substrateMoistureTargetBuffer, "%");

  
  updateWatering(atmosTemp, substrateMoisture);

  if (Serial1.available())
  {
    String input = Serial1.readStringUntil('\n');
    input.trim();

    if (input == "{\"request\": \"realtime\"}")
    {
      transmitData(timestamp, solutionTemp, substrateTemp, substrateMoisture, atmosTemp, atmosHumidity);
    }

    else if (input == "GET_CONFIG") {
      transmitConfig();
    }
    else if (input == "{\"request\": \"historical\"}") {
      transmitDataHistorical(timestamp, solutionTemp, substrateTemp, substrateMoisture, atmosTemp, atmosHumidity);
    }
  }

  if (currentTime - lastDisplayUpdate >= displayRefreshInterval)
  {
    draw();
    lastDisplayUpdate = currentTime;
  }

  if (currentTime - lastLogTime >= logInterval)
  {
    logDataToSD(timestamp, solutionTemp, substrateTemp, substrateMoisture, atmosTemp, atmosHumidity);
    lastLogTime = currentTime;
  }
}