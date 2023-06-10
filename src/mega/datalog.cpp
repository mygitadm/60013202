#include <TimeLib.h>

#include "datalog.h"

void initializeSDCard()
{
  if (!SD.begin(chipSelect))
  {
    Serial.println("Card failed, or not present");
    sdCardPresent = false;
  }
  else
  {
    Serial.println("Card initialized.");
    sdCardPresent = true;
    if (!SD.exists("infoLog.csv"))
    { // Check if file already exists
      File dataFile = SD.open("infoLog.csv", FILE_WRITE);
      if (dataFile)
      {
        dataFile.println("timestamp,solutionTemp,substrateTemp,atmosTemp,atmosHumidity,substrateMoisturePercent"); // Write the headers
        dataFile.close();
      }
    }
  }
}

void logDataToSD(float solutionTemp, float substrateTemp, float substrateMoisturePercent, float atmosTemp, float atmosHumidity)
{
  if (!sdCardPresent || fileInUse)
  {
    return;
  }

  fileInUse = true;

  // Check if the file exists and create it with headers if it doesn't
  if (!SD.exists("infoLog.csv"))
  {
    File dataFile = SD.open("infoLog.csv", FILE_WRITE);
    if (dataFile)
    {
      dataFile.println("Timestamp,SolutionTemp,SubstrateTemp,AtmosTemp,AtmosHumidity,SubstrateMoisturePercent");
      dataFile.close();
    }
    else
    {
      Serial.println("error creating infoLog.csv");
      return; // If the file couldn't be created, exit the function
    }
  }

  File dataFile = SD.open("infoLog.csv", FILE_WRITE);

  if (dataFile)
  {
    // ISO 8601 timestamp format
    char timestamp[25];
    snprintf(timestamp, sizeof(timestamp), "%04d-%02d-%02dT%02d:%02d",
             tmYearToCalendar(tm.Year), tm.Month, tm.Day, tm.Hour, tm.Minute);

    // CSV format
    dataFile.print(String(timestamp) + ",");
    dataFile.print(String(solutionTemp) + ",");
    dataFile.print(String(substrateTemp) + ",");
    dataFile.print(String(atmosTemp) + ",");
    dataFile.print(String(atmosHumidity) + ",");
    dataFile.println(String(substrateMoisturePercent));

    dataFile.close();
  }
  else
  {
    Serial.println("error opening infoLog.csv");
  }
  fileInUse = false;
}