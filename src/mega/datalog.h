#ifndef DATALOG_H
#define DATALOG_H

#include <SPI.h>
#include <SD.h>


extern unsigned long lastLogTime;
extern const unsigned long logInterval;
extern bool sdCardPresent;
extern const int chipSelect;
extern File infoLog;
extern bool fileInUse;

extern tmElements_t tm;


void initializeSDCard();
void logDataToSD(time_t timestamp, float solutionTemp, float substrateTemp, float substrateMoisturePercent, float atmosTemp, float atmosHumidity);

#endif