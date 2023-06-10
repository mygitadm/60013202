#include <Arduino.h>
#include "sensors.h"

char tempBuffer[TEMP_BUFFER_SIZE];
char humidBuffer[HUMID_BUFFER_SIZE];
char tempSubstrateBuffer[SUBSTRATE_SENSOR_BUFFER_SIZE];
char tempSolutionBuffer[SOLUTION_SENSOR_BUFFER_SIZE];
char substrateMoistureBuffer[SUBSTRATE_MOISTURE_BUFFER_SIZE];
char substrateMoistureTargetBuffer[SUBSTRATE_MOISTURE_TARGET_BUFFER_SIZE];

Adafruit_BME280 bme;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress substrateTempSensor = {0x28, 0xED, 0x2E, 0x3C, 0x05, 0x00, 0x00, 0x48};
DeviceAddress solutionTempSensor = {0x28, 0xB7, 0x1B, 0x3D, 0x05, 0x00, 0x00, 0x70};

//======MOISTURE_SENSOR=====//
//#define MOISTURE_SENSOR A15
const int AirValue = 542;   // free air
const int WaterValue = 211; // water
int minMoistureSetPoint = 0; // set min moisture
int maxMoistureSetPoint = 0; // set max moisture
//==============//

//=======Amtos temp and substrate moisture range to calc target moisture =====//

const float minTemp = 15; // replace with the minimum temperature for humidity regulation
const float maxTemp = 30; // replace with the maximum temperature for humidity regulation
const int minHumidity = 40; // replace with the minimum humidity for low temperature
const int maxHumidity = 85; // replace with the maximum humidity for high temperature

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
