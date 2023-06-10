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

// Define display refresh rate
unsigned long lastDisplayUpdate = 0;
const unsigned long displayRefreshInterval = 250;

// tmElements_t tm;
const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
/*
//// Objects for handling sensors
//============BME280===============//
Adafruit_BME280 bme;
//============DS18B20============//
#define ONE_WIRE_BUS 26
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress substrateTempSensor = {0x28, 0xED, 0x2E, 0x3C, 0x05, 0x00, 0x00, 0x48};
DeviceAddress solutionTempSensor = {0x28, 0xB7, 0x1B, 0x3D, 0x05, 0x00, 0x00, 0x70};
*/


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

/*
//=======Amtos temp and substrate moisture range to calc target moisture =====//

const float minTemp = 15; // replace with the minimum temperature for humidity regulation
const float maxTemp = 30; // replace with the maximum temperature for humidity regulation
const int minHumidity = 40; // replace with the minimum humidity for low temperature
const int maxHumidity = 85; // replace with the maximum humidity for high temperature
*/
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
void transmitData(float solutionTemp, float substrateTemp, float substrateMoisturePercent, float atmosTemp, float atmosHumidity);
void logDataToSD(float solutionTemp, float substrateTemp, float substrateMoisturePercent, float atmosTemp, float atmosHumidity);
void setupPump();
void setupDisplay();
void draw();

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
  setupRTC();
  setupSensors();
  pinMode(MOISTURE_SENSOR, INPUT);
  initializeSDCard();
  setupPump();
  pinMode(relayPin, OUTPUT);
  //u8g2.begin();
  setupDisplay();
}
/*
void setupSensors()
{
  sensors.begin();
  sensors.setWaitForConversion(false); // Make temperature requests asynchronous
  // Set the resolution
  sensors.setResolution(substrateTempSensor, 9);
  sensors.setResolution(solutionTempSensor, 9);
  // BME280 sensor
  if (!bme.begin(0x77))
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
  }

  Serial.println("Sensors are detected and ready to use.");
}
*/
void setupPump() {
  // Initialize motor control pins
  pinMode(MOTOR_SPEED_PIN, OUTPUT);
  pinMode(MOTOR_CONTROL_PIN1, OUTPUT);
  pinMode(MOTOR_CONTROL_PIN2, OUTPUT);
  
  // Set default speed to 100%
  analogWrite(MOTOR_SPEED_PIN, 255);
}
/*
void getBME280Data(float &atmosTemp, float &atmosHumidity)
{
  atmosTemp = bme.readTemperature();
  atmosHumidity = bme.readHumidity();

  // Use the new function to format the sensor data
  formatSensorData(atmosTemp, tempBuffer, "C");
  formatSensorData(atmosHumidity, humidBuffer, "%");
}

void getDS18B20Data(float &substrateTemp, float &solutionTemp)
{
  sensors.requestTemperatures(); // Send the command to get temperatures
  substrateTemp = sensors.getTempC(substrateTempSensor);
  solutionTemp = sensors.getTempC(solutionTempSensor);

  // Use the new function to format the sensor data
  formatSensorData(substrateTemp, tempSubstrateBuffer, "C");
  formatSensorData(solutionTemp, tempSolutionBuffer, "C");
}

void getSubstrateMoisture(float &substrateMoisture)
{
  int substrateMoistureValue = analogRead(MOISTURE_SENSOR);
  substrateMoisture = map(substrateMoistureValue, AirValue, WaterValue, 0, 100);
  substrateMoisture = constrain(substrateMoisture, 0, 100);

  // Use the new function to format the sensor data
  formatSensorData(substrateMoisture, substrateMoistureBuffer, "%");
}
*/
void transmitData(float solutionTemp, float substrateTemp, float substrateMoisturePercent, float atmosTemp, float atmosHumidity)
{
  time_t timestamp = now();

  // Create JSON document
  StaticJsonDocument<200> doc;

  // Add values to JSON document
  doc["dataType"] = "realtime";
  doc["Timestamp"] = timestamp;
  doc["Temp1"] = solutionTemp;
  doc["Temp2"] = substrateTemp;
  doc["BME280 Temp"] = atmosTemp;
  doc["BME280 Humidity"] = atmosHumidity;
  doc["Soil Moisture"] = substrateMoisturePercent;

  // Transmit JSON via Serial to ESP8266
  serializeJson(doc, Serial1);
  Serial1.println();

  // Print the same JSON to Serial for debugging
  serializeJson(doc, Serial);
  Serial.println();
}
/*
void formatSensorData(float value, char *buffer, const char* unit) {
    dtostrf(value, 4, 1, buffer);
    strcat(buffer, unit);
}

float getTargetHumidity(float atmosTemp) {
  if (atmosTemp <= minTemp) {
    return minHumidity;
  } else if (atmosTemp >= maxTemp) {
    return maxHumidity;
  } else {
    return minHumidity + (maxHumidity - minHumidity) * ((atmosTemp - minTemp) / (maxTemp - minTemp));
  }
}
*/
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

    if (input == "{\"request\": \"data\"}")
    {
      transmitData(solutionTemp, substrateTemp, substrateMoisture, atmosTemp, atmosHumidity);
    }

    else if (input == "GET_CONFIG") {
      transmitConfig();
    }
  }

  if (currentTime - lastDisplayUpdate >= displayRefreshInterval)
  {
    draw();
    lastDisplayUpdate = currentTime;
  }

  if (currentTime - lastLogTime >= logInterval)
  {
    logDataToSD(solutionTemp, substrateTemp, substrateMoisture, atmosTemp, atmosHumidity);
    lastLogTime = currentTime;
  }
}