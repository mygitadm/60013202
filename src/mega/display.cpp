#include "display.h"

//=========ST7920=========//
U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, /* clock=*/ 22, /* data=*/ 23, /* CS=*/ 24, /* reset=*/ 25);

const uint8_t TIME_POS_X = 0;
const uint8_t TIME_POS_Y = 12;
const uint8_t DATE_POS_X = 48;
const uint8_t DATE_POS_Y = 12;

const uint8_t ATMOS_HUMIDITY_POS_X = 10;
const uint8_t ATMOS_HUMIDITY_POS_Y = 24;
const uint8_t ATMOS_TEMP_POS_X = 60;
const uint8_t ATMOS_TEMP_POS_Y = 24;

const uint8_t SUBSTRATE_MOISTURE_TARGET_POS_X = 10;
const uint8_t SUBSTRATE_MOISTURE_TARGET_POS_Y = 36;

const uint8_t SUBSTRATE_TEMP_POS_X = 10;
const uint8_t SUBSTRATE_TEMP_POS_Y = 48;
const uint8_t SOLUTION_TEMP_POS_X = 60;
const uint8_t SOLUTION_TEMP_POS_Y = 48;

const uint8_t SUBSTRATE_MOISTURE_POS_X = 10;
const uint8_t SUBSTRATE_MOISTURE_POS_Y = 60;

void setupDisplay()
{
  u8g2.begin();
}

void draw()
{
  u8g2.firstPage();
  do
  {
    u8g2.setFont(u8g_font_unifont);
    u8g2.drawStr(TIME_POS_X, TIME_POS_Y, timeBuffer);
    u8g2.drawStr(DATE_POS_X, DATE_POS_Y, dateString);
    u8g2.drawStr(ATMOS_HUMIDITY_POS_X, ATMOS_HUMIDITY_POS_Y, humidBuffer);
    u8g2.drawStr(ATMOS_TEMP_POS_X, ATMOS_TEMP_POS_Y, tempBuffer);
    u8g2.drawStr(SUBSTRATE_MOISTURE_TARGET_POS_X, SUBSTRATE_MOISTURE_TARGET_POS_Y, substrateMoistureTargetBuffer);
    u8g2.drawStr(SUBSTRATE_TEMP_POS_X, SUBSTRATE_TEMP_POS_Y, tempSubstrateBuffer);
    u8g2.drawStr(SOLUTION_TEMP_POS_X, SOLUTION_TEMP_POS_Y, tempSolutionBuffer);
    u8g2.drawStr(SUBSTRATE_MOISTURE_POS_X, SUBSTRATE_MOISTURE_POS_Y, substrateMoistureBuffer);
  } while (u8g2.nextPage());
}
