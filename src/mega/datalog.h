#ifndef DATALOG_H
#define DATALOG_H

#include <SPI.h>
#include <SD.h>

extern const unsigned long logInterval;
extern bool sdCardPresent;
extern bool fileInUse;
extern const int chipSelect;
extern File infoLog;

extern tmElements_t tm;


void initializeSDCard();
void logDataToSD(float solutionTemp, float substrateTemp, float substrateMoisturePercent, float atmosTemp, float atmosHumidity);

#endif