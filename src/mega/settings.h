// settings.h

#ifndef SETTINGS_H
#define SETTINGS_H

//======MOISTURE_SENSOR=====//
//#define MOISTURE_SENSOR A15
extern const int AirValue;   // free air
extern const int WaterValue; // water

// Humidity settings
extern int minMoistureSetPoint; // set min moisture
extern int maxMoistureSetPoint; // set max moisture

// Temperature settings
extern const float minTemp; // replace with the minimum temperature for humidity regulation
extern const float maxTemp; // replace with the maximum temperature for humidity regulation

// Humidity settings
//extern const int minHumidity; // replace with the minimum humidity for low temperature
//extern const int maxHumidity; // replace with the maximum humidity for high temperature

// Light intensity settings
extern const float minLightIntensity; // replace with the minimum light intensity for humidity regulation
extern const float maxLightIntensity; // replace with the maximum light intensity for humidity regulation



// Humidity settings for each growth stage
extern  int minHumiditySeedling; 
extern  int maxHumiditySeedling; 
extern  int minHumidityVegetative; 
extern  int maxHumidityVegetative; 
extern  int minHumidityFruitBearing; 
extern  int maxHumidityFruitBearing; 

extern int currentGrowthStage;



#endif // SETTINGS_H