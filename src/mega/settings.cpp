// settings.cpp

#include "settings.h"

//======MOISTURE_SENSOR=====//
//#define MOISTURE_SENSOR A15
const int AirValue = 542;   // free air
const int WaterValue = 211; // water

// Humidity settings
int minMoistureSetPoint = 0; // set min moisture
int maxMoistureSetPoint = 0; // set max moisture

// Temperature settings
const float minTemp = 15.0; // replace with the minimum temperature for humidity regulation
const float maxTemp = 30.0; // replace with the maximum temperature for humidity regulation

// Humidity settings
const int minHumidity = 40; // replace with the minimum humidity for low temperature
const int maxHumidity = 85; // replace with the maximum humidity for high temperature
