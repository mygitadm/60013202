#include <Arduino.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include "RTC.h"

tmElements_t tm;
const uint8_t TIME_BUFFER_SIZE = 9;
const uint8_t DATE_BUFFER_SIZE = 20;
char timeBuffer[TIME_BUFFER_SIZE];
char dateString[DATE_BUFFER_SIZE];

void setupRTC()
{
  setSyncProvider(RTC.get);
  if (timeStatus() != timeSet)
  {
    Serial.println("Unable to sync with the RTC");
  }
  else
  {
    Serial.println("RTC has set the system time");
  }
}

bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%02d:%02d:%02d", &Hour, &Min, &Sec) != 3)
  {
    return false;
  }

  if (Hour < 0 || Hour > 23 || Min < 0 || Min > 59 || Sec < 0 || Sec > 59)
  {
    return false;
  }

  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;

  return true;
}

bool getDate(const char *str)
{
  int Day, Month, Year;

  if (sscanf(str, "%02d/%02d/%04d", &Day, &Month, &Year) != 3)
  {
    return false;
  }

  if (Day < 1 || Day > 31 || Month < 1 || Month > 12 || Year < 1900 || Year > 2100)
  {
    return false;
  }

  tm.Day = Day;
  tm.Month = Month;
  tm.Year = CalendarYrToTm(Year);

  return true;
}

void getDateTime()
{
  RTC.read(tm);

  snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d", tm.Hour, tm.Minute);
  //snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d", tm.Hour, tm.Minute, tm.Second);
  snprintf(dateString, sizeof(dateString), "%02d/%02d/%04d", tm.Month, tm.Day, tmYearToCalendar(tm.Year));
  //snprintf(dateString, sizeof(dateString), "%s %02d, %04d", monthName[tm.Month - 1], tm.Day, tmYearToCalendar(tm.Year));
}