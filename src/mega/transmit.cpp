#include "transmit.h"
#include "settings.h"

void transmitData(time_t timestamp, float solutionTemp, float substrateTemp, float substrateMoisturePercent, float atmosTemp, float atmosHumidity)
{
  char buffer[6];
  // Create JSON document
  StaticJsonDocument<200> doc;

  // Add values to JSON document
  doc["dataType"] = "realtime";
  doc["timestamp"] = timestamp;
  doc["solutionTemp"] = solutionTemp;
  doc["substrateTemp"] = substrateTemp;
  doc["atmosTemp"] = atmosTemp;
  //doc["atmosHumidity"] = atmosHumidity;
  dtostrf(atmosHumidity, 4, 1, buffer);
  doc["atmosHumidity"] = buffer;
  doc["substrateMoisture"] = substrateMoisturePercent;

  // Transmit JSON via Serial to ESP8266
  serializeJson(doc, Serial1);
  Serial1.println();

  // Print the same JSON to Serial for debugging
  serializeJson(doc, Serial);
  Serial.println();
}

void transmitDataHistorical(time_t timestamp, float solutionTemp, float substrateTemp, float substrateMoisturePercent, float atmosTemp, float atmosHumidity)
{
  char buffer[6];

  // Create JSON document
  StaticJsonDocument<200> doc;
  
  // Add values to JSON document
  doc["dataType"] = "historical";
  doc["timestamp"] = timestamp;
  doc["solutionTemp"] = solutionTemp;
  doc["substrateTemp"] = substrateTemp;
  doc["atmosTemp"] = atmosTemp;
  dtostrf(atmosHumidity, 4, 1, buffer);
  doc["atmosHumidity"] = buffer;
  doc["substrateMoisture"] = substrateMoisturePercent;

  // Transmit JSON via Serial to ESP8266
  serializeJson(doc, Serial1);
  Serial1.println();

  // Print the same JSON to Serial for debugging
  serializeJson(doc, Serial);
  Serial.println();
}


void transmitConfig() {
  // Create JSON document
  StaticJsonDocument<200> doc;

  // Add values to JSON document
  doc["dataType"] = "config";
  doc["minTemp"] = minTemp;
  doc["maxTemp"] = maxTemp;
  doc["minHumidity"] = minHumidity;
  doc["maxHumidity"] = maxHumidity;

  // Transmit JSON via Serial1 to ESP8266
  serializeJson(doc, Serial1);
  Serial1.println();

  // Print the same JSON to Serial for debugging
  serializeJson(doc, Serial);
  Serial.println();
}