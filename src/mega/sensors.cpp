#include <Arduino.h>
#include "sensors.h"
#include "settings.h"


#define N 10 // Number of measurements to average
float measurements[N] = {0};
int current_index = 0;

char tempBuffer[TEMP_BUFFER_SIZE];
char humidBuffer[HUMID_BUFFER_SIZE];
char tempSubstrateBuffer[SUBSTRATE_SENSOR_BUFFER_SIZE];
char tempSolutionBuffer[SOLUTION_SENSOR_BUFFER_SIZE];
char substrateMoistureBuffer[SUBSTRATE_MOISTURE_BUFFER_SIZE];
char substrateMoistureTargetBuffer[SUBSTRATE_MOISTURE_TARGET_BUFFER_SIZE];
char lightIntensityBuffer[LIGHTINTENSITY_BUFFER_SIZE];

Adafruit_BME280 bme;

ClosedCube_OPT3001 opt3001;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress substrateTempSensor = {0x28, 0xED, 0x2E, 0x3C, 0x05, 0x00, 0x00, 0x48};
DeviceAddress solutionTempSensor = {0x28, 0xB7, 0x1B, 0x3D, 0x05, 0x00, 0x00, 0x70};


void setupSensors()
{
  sensors.begin();
  sensors.setWaitForConversion(false); // Make temperature requests asynchronous
  // Set the resolution
  sensors.setResolution(substrateTempSensor, 9);
  sensors.setResolution(solutionTempSensor, 9);

  opt3001.begin(OPT3001_ADDRESS);
	Serial.print("OPT3001 Manufacturer ID");
	Serial.println(opt3001.readManufacturerID());
	Serial.print("OPT3001 Device ID");
	Serial.println(opt3001.readDeviceID());

	configureOPT3001Sensor();
	//printResult("High-Limit", opt3001.readHighLimit());
	//printResult("Low-Limit", opt3001.readLowLimit());
	Serial.println("----");
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

  // Add the new measurement to the array and update the index
  measurements[current_index] = substrateMoisture;
  current_index = (current_index + 1) % N;

  // Compute the moving average
  float sum = 0;
  for (int i = 0; i < N; i++) {
    sum += measurements[i];
  }
  substrateMoisture = sum / N;

  // Use the new function to format the sensor data
  formatSensorData(substrateMoisture, substrateMoistureBuffer, "%");
}

void formatSensorData(float value, char *buffer, const char* unit) {
    dtostrf(value, 4, 1, buffer);
    strcat(buffer, unit);
}
void getSubstrateMoistureTarget(float atmosTemp, float lightIntensity, float &targetMoisture) {
  int minHumidity, maxHumidity;
  
  // Choose the correct humidity settings based on the current growth stage
  if (currentGrowthStage == 1) {
    minHumidity = minHumiditySeedling;
    maxHumidity = maxHumiditySeedling;
  } else if (currentGrowthStage == 2) {
    minHumidity = minHumidityVegetative;
    maxHumidity = maxHumidityVegetative;
  } else if (currentGrowthStage == 3) {
    minHumidity = minHumidityFruitBearing;
    maxHumidity = maxHumidityFruitBearing;
  }
  // Calculate the target humidity based on temperature as before
  float targetHumidityTemp;
  if (atmosTemp <= minTemp) {
    targetHumidityTemp = minHumidity;
  } else if (atmosTemp >= maxTemp) {
    targetHumidityTemp = maxHumidity;
  } else {
    targetHumidityTemp = minHumidity + (maxHumidity - minHumidity) * ((atmosTemp - minTemp) / (maxTemp - minTemp));
  }

  // Now calculate the target humidity based on light intensity
  float targetHumidityLight = minHumidity + (maxHumidity - minHumidity) * (lightIntensity / 65000);
  //float targetHumidityLight = minHumidity + (maxHumidity - minHumidity) * pow(lightIntensity / 65000.0, 2);


	// Combine the two target humidities by taking a weighted average
	targetMoisture = 0.6 * targetHumidityTemp + 0.4 * targetHumidityLight;


  formatSensorData(targetMoisture, substrateMoistureTargetBuffer, "%");
}


void getOPT3001Data(float &lightIntensity)
{
    lightIntensity = opt3001.readResult().lux;
    formatSensorData(lightIntensity, lightIntensityBuffer, "LUX");
    //dtostrf(lightIntensity, 6, 2, lightIntensityBuffer);  // Convert float to string, adjust as needed
}


void configureOPT3001Sensor() {
	OPT3001_Config newConfig;
	
	newConfig.RangeNumber = B1100;	
	newConfig.ConvertionTime = B0;
	newConfig.Latch = B1;
	newConfig.ModeOfConversionOperation = B11;

	OPT3001_ErrorCode errorConfig = opt3001.writeConfig(newConfig);
	if (errorConfig != NO_ERROR)
    Serial.println("OPT3001 configuration");
		//printError("OPT3001 configuration", errorConfig);
	else {
		OPT3001_Config sensorConfig = opt3001.readConfig();
		Serial.println("OPT3001 Current Config:");
		Serial.println("------------------------------");
		
		Serial.print("Conversion ready (R):");
		Serial.println(sensorConfig.ConversionReady,HEX);

		Serial.print("Conversion time (R/W):");
		Serial.println(sensorConfig.ConvertionTime, HEX);

		Serial.print("Fault count field (R/W):");
		Serial.println(sensorConfig.FaultCount, HEX);

		Serial.print("Flag high field (R-only):");
		Serial.println(sensorConfig.FlagHigh, HEX);

		Serial.print("Flag low field (R-only):");
		Serial.println(sensorConfig.FlagLow, HEX);

		Serial.print("Latch field (R/W):");
		Serial.println(sensorConfig.Latch, HEX);

		Serial.print("Mask exponent field (R/W):");
		Serial.println(sensorConfig.MaskExponent, HEX);

		Serial.print("Mode of conversion operation (R/W):");
		Serial.println(sensorConfig.ModeOfConversionOperation, HEX);

		Serial.print("Polarity field (R/W):");
		Serial.println(sensorConfig.Polarity, HEX);

		Serial.print("Overflow flag (R-only):");
		Serial.println(sensorConfig.OverflowFlag, HEX);

		Serial.print("Range number (R/W):");
		Serial.println(sensorConfig.RangeNumber, HEX);

		Serial.println("------------------------------");
	}
	
}
/*
void printResult(String text, OPT3001 result) {
	if (result.error == NO_ERROR) {
		Serial.print(text);
		Serial.print(": ");
		Serial.print(result.lux);
		Serial.println(" lux");
	}
	else {
		printError(text,result.error);
	}
}

void printError(String text, OPT3001_ErrorCode error) {
	Serial.print(text);
	Serial.print(": [ERROR] Code #");
	Serial.println(error);
}
*/

