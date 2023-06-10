#ifndef RTC_H
#define RTC_H

#include <TimeLib.h>
#include <DS1307RTC.h>

extern tmElements_t tm;
extern const uint8_t TIME_BUFFER_SIZE;
extern const uint8_t DATE_BUFFER_SIZE;
extern char timeBuffer[];
extern char dateString[];

void setupRTC();
bool getTime(const char *str);
bool getDate(const char *str);
void getDateTime();

#endif


