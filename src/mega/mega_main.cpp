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
#include "transmit.h"
#include "pump.h"
/*
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



void setupPump();
*/

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
  setupDisplay();
}
/*
void setupPump() {
  // Initialize motor control pins
  pinMode(MOTOR_SPEED_PIN, OUTPUT);
  pinMode(MOTOR_CONTROL_PIN1, OUTPUT);
  pinMode(MOTOR_CONTROL_PIN2, OUTPUT);
  
  // Set default speed to 100%
  analogWrite(MOTOR_SPEED_PIN, 255);
}


void updateWatering(float atmosTemp, float substrateMoisture) {
  float targetHumidity;
  getSubstrateMoistureTarget(atmosTemp, targetHumidity);
  if (substrateMoisture < targetHumidity) {
    digitalWrite(relayPin, HIGH);
  } else {
    digitalWrite(relayPin, LOW);
  }
}
*/
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

  float substrateMoistureTarget;
  getSubstrateMoistureTarget(atmosTemp, substrateMoistureTarget);

  float lightIntensity;
  getOPT3001Data(lightIntensity);
  /*Serial.print("Light Intensity: ");
  Serial.print(lightIntensity);
  Serial.println(" lux");*/
  
  updateWatering(atmosTemp, substrateMoisture);

  updateDisplay();

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
  if (currentTime - lastLogTime >= logInterval)
  {
    logDataToSD(timestamp, solutionTemp, substrateTemp, substrateMoisture, atmosTemp, atmosHumidity);
    lastLogTime = currentTime;
  }
}