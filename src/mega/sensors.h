#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_BME280.h>
#include <ClosedCube_OPT3001.h>
#include <PID_v1.h>

//opt3001
extern ClosedCube_OPT3001 opt3001;
#define OPT3001_ADDRESS 0x44

// Define the OneWire bus pin
#define ONE_WIRE_BUS 26
extern OneWire oneWire;

#define MOISTURE_SENSOR A15

extern const int AirValue;   // free air
extern const int WaterValue; // water
extern int minMoistureSetPoint; // set min moisture
extern int maxMoistureSetPoint;// set max moisture

extern const float minTemp; // replace with the minimum temperature for humidity regulation
extern const float maxTemp; // replace with the maximum temperature for humidity regulation
//extern const int minHumidity; // replace with the minimum humidity for low temperature
//extern const int maxHumidity; // replace with the maximum humidity for high temperature


// Define the size of the temperature and humidity buffers BME280
const uint8_t TEMP_BUFFER_SIZE = 7; // including space for units and null terminator
const uint8_t HUMID_BUFFER_SIZE = 7;
extern char tempBuffer[TEMP_BUFFER_SIZE];
extern char humidBuffer[HUMID_BUFFER_SIZE];

// Define the size of the temperature buffers DS18B20
const uint8_t SUBSTRATE_SENSOR_BUFFER_SIZE = 7; // including space for units and null terminator
const uint8_t SOLUTION_SENSOR_BUFFER_SIZE = 7;
extern char tempSubstrateBuffer[SUBSTRATE_SENSOR_BUFFER_SIZE];
extern char tempSolutionBuffer[SOLUTION_SENSOR_BUFFER_SIZE];

// Define the size of substrate moisture buffer for capacitive moisture sensor
const uint8_t SUBSTRATE_MOISTURE_BUFFER_SIZE = 7; // including space for units and null terminator
extern char substrateMoistureBuffer[SUBSTRATE_MOISTURE_BUFFER_SIZE];

// Define the size of the target moisture buffer
const uint8_t SUBSTRATE_MOISTURE_TARGET_BUFFER_SIZE = 7; // including space for units and null terminator
extern char substrateMoistureTargetBuffer[SUBSTRATE_MOISTURE_TARGET_BUFFER_SIZE];

// Define the size of the light intensuty buffer
const uint8_t LIGHTINTENSITY_BUFFER_SIZE = 10; // Adjust as needed
extern char lightIntensityBuffer[LIGHTINTENSITY_BUFFER_SIZE];

// Objects for handling sensors
extern Adafruit_BME280 bme;
extern OneWire oneWire;
extern DallasTemperature sensors;
extern DeviceAddress substrateTempSensor;
extern DeviceAddress solutionTempSensor;

void setupSensors();
void getBME280Data(float &atmosTemp, float &atmosHumidity);
void getDS18B20Data(float &substrateTemp, float &solutionTemp);
void getSubstrateMoisture(float &substrateMoisture);
void formatSensorData(float value, char *buffer, const char* unit);
void getSubstrateMoistureTarget(float atmosTemp, float lightIntensity, float &targetMoisture);
void getOPT3001Data(float &lightIntensity);
void configureOPT3001Sensor();
//void printResult();
//void printError();


extern double Input;
extern double Setpoint;

#endif // SENSORS_H
