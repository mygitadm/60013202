    #include "settings.h"

    //======MOISTURE_SENSOR=====//
    //#define MOISTURE_SENSOR A15
    const int AirValue = 542;   // free air
    const int WaterValue = 211; // water

    // future remote Humidity settings
    int minMoistureSetPoint = 0; // set min moisture
    int maxMoistureSetPoint = 0; // set max moisture

    // Temperature settings
    const float minTemp = 15.0; // replace with the minimum temperature for humidity regulation
    const float maxTemp = 30.0; // replace with the maximum temperature for humidity regulation

    // Humidity settings
    //const int minHumidity = 40; // replace with the minimum humidity for low temperature
    //const int maxHumidity = 85; // replace with the maximum humidity for high temperature

    // Light intensity settings
    const float minLightIntensity = 0.0; // replace with the minimum light intensity for humidity regulation
    const float maxLightIntensity = 1000.0; // replace with the maximum light intensity for humidity regulation

    // Humidity settings for each growth stage
    int minHumiditySeedling = 20; 
    int maxHumiditySeedling = 40; 
    int minHumidityVegetative = 30; 
    int maxHumidityVegetative = 50; 
    int minHumidityFruitBearing = 40; 
    int maxHumidityFruitBearing = 60; 

    int currentGrowthStage = 2; // Start at seedling stage 1-Seedling stage, 2-Vegetative stage,3-Fruit-bearing stage

