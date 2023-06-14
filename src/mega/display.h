#ifndef DISPLAY_H
#define DISPLAY_H

#include <U8g2lib.h>

extern const uint8_t TIME_POS_X;
extern const uint8_t TIME_POS_Y;
extern const uint8_t DATE_POS_X;
extern const uint8_t DATE_POS_Y;

extern const uint8_t ATMOS_HUMIDITY_POS_X;
extern const uint8_t ATMOS_HUMIDITY_POS_Y;
extern const uint8_t ATMOS_TEMP_POS_X;
extern const uint8_t ATMOS_TEMP_POS_Y;

extern const uint8_t SUBSTRATE_MOISTURE_TARGET_POS_X;
extern const uint8_t SUBSTRATE_MOISTURE_TARGET_POS_Y;

extern const uint8_t SUBSTRATE_TEMP_POS_X;
extern const uint8_t SUBSTRATE_TEMP_POS_Y;
extern const uint8_t SOLUTION_TEMP_POS_X;
extern const uint8_t SOLUTION_TEMP_POS_Y;

extern const uint8_t SUBSTRATE_MOISTURE_POS_X;
extern const uint8_t SUBSTRATE_MOISTURE_POS_Y;

extern const uint8_t LIGHTINTENSITY_POS_X;
extern const uint8_t LIGHTINTENSITY_POS_Y;

extern const uint8_t TIME_BUFFER_SIZE;
extern const uint8_t DATE_BUFFER_SIZE;
extern char timeBuffer[];
extern char dateString[];

extern const uint8_t TEMP_BUFFER_SIZE;
extern const uint8_t HUMID_BUFFER_SIZE;
extern char tempBuffer[];
extern char humidBuffer[];

extern const uint8_t SUBSTRATE_SENSOR_BUFFER_SIZE;
extern const uint8_t SOLUTION_SENSOR_BUFFER_SIZE;
extern char tempSubstrateBuffer[];
extern char tempSolutionBuffer[];

extern const uint8_t SUBSTRATE_MOISTURE_BUFFER_SIZE;
extern char substrateMoistureBuffer[];

extern const uint8_t SUBSTRATE_MOISTURE_TARGET_BUFFER_SIZE;
extern char substrateMoistureTargetBuffer[];

extern const uint8_t LIGHTINTENSITY_BUFFER_SIZE;
extern char lightIntensityBuffer[];

void draw();

#endif
