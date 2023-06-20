#ifndef TRANSMIT_H
#define TRANSMIT_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <TimeLib.h>

void transmitData(time_t timestamp, float solutionTemp, float substrateTemp, float substrateMoisturePercent, float atmosTemp, float atmosHumidity);
void transmitDataHistorical(time_t timestamp, float solutionTemp, float substrateTemp, float substrateMoisturePercent, float atmosTemp, float atmosHumidity);
void transmitConfig();

#endif // TRANSMIT_H
