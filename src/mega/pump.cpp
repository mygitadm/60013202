#include "pump.h"
#include "sensors.h"
#include "datalog.h"
#include <TimeLib.h>
#include <PID_v1.h>

double Setpoint, Input, Output;
double Kp=1, Ki=0, Kd=0;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);



unsigned long lastPumpOnTime = 0;
const unsigned long pumpOnInterval = 60000;

unsigned long startTime = millis();
const int MOTOR_SPEED_PIN = 44;
const int MOTOR_CONTROL_PIN1 = 43;
const int MOTOR_CONTROL_PIN2 = 42;


void setupPump() {
  // Initialize motor control pins
  pinMode(MOTOR_SPEED_PIN, OUTPUT);
  pinMode(MOTOR_CONTROL_PIN1, OUTPUT);
  pinMode(MOTOR_CONTROL_PIN2, OUTPUT);
  
  // Set default speed to 100%
  analogWrite(MOTOR_SPEED_PIN, 255);

   // Initialize PID
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0, 255); // This depends on the range of your pump
}


void controlPump(bool state) {
  if (state) {
    digitalWrite(MOTOR_CONTROL_PIN1, HIGH); // Turn ON the water pump
    digitalWrite(MOTOR_CONTROL_PIN2, LOW);
  } else {
    digitalWrite(MOTOR_CONTROL_PIN1, LOW); // Turn OFF the water pump
    digitalWrite(MOTOR_CONTROL_PIN2, LOW);
  }
}

void controlRelay(bool state) {
  if (state) {
    digitalWrite(relayPin, HIGH); // Turn ON the relay
  } else {
    digitalWrite(relayPin, LOW); // Turn OFF the relay
  }
}

void updateWatering(float atmosTemp, float substrateMoisture, float lightIntensity) {
  const unsigned long pumpInterval = 5000; // Time for the pump to be ON in milliseconds
  const unsigned long pauseInterval = 60000; // Pause time in milliseconds
  static unsigned long lastPumpStartTime = 0; // Time when pump was last turned ON

  float targetHumidity;
  float hysteresis = 3.0;
  static float initialMoisture;
  static bool pumpState = false;
  unsigned long currentTime = millis();

  getSubstrateMoistureTarget(atmosTemp, lightIntensity, targetHumidity);

  if(currentTime - startTime > 30000) {
    if(!pumpState && substrateMoisture < targetHumidity - hysteresis && currentTime - lastPumpStartTime >= pauseInterval) {
      controlPump(true);
      lastPumpStartTime = currentTime;
      pumpState = true;
      initialMoisture = substrateMoisture;
      Serial.print("Target Moisture: ");
      Serial.println(targetHumidity);
      Serial.print("Pump turned ON at humidity: ");
      Serial.println(substrateMoisture);

      float solutionTemp, substrateTemp;
      getDS18B20Data(solutionTemp, substrateTemp);

      float atmosTemp, atmosHumidity;
      getBME280Data(atmosTemp, atmosHumidity);

      logPumpDataToSD(currentTime, solutionTemp, substrateTemp, atmosTemp, atmosHumidity, substrateMoisture, lightIntensity);
    } 
    else if(pumpState && (currentTime - lastPumpStartTime >= pumpInterval || substrateMoisture >= initialMoisture + 2)) {
      controlPump(false);
      pumpState = false;
      Serial.print("Target Moisture: ");
      Serial.println(targetHumidity);
      Serial.print("Pump turned OFF at humidity: ");
      Serial.println(substrateMoisture);
    }
  }
}


/*
v4
void updateWatering(float atmosTemp, float substrateMoisture, float lightIntensity) {
  float targetHumidity;
  float hysteresis = 3.0; // Adjust as needed

  static float initialMoisture; // Store the initial moisture level
  static bool pumpState = false; // Keep track of the pump state

  static unsigned long pumpOffTime = 0; // Store the last time the pump was turned off
 

  unsigned long currentTime = millis(); // Get the current time

  getSubstrateMoistureTarget(atmosTemp, lightIntensity, targetHumidity);

  if (currentTime - startTime > 30000) { // Wait for 5 seconds after reboot
    // Existing pump control logic
    if (!pumpState && substrateMoisture < targetHumidity - hysteresis && currentTime - pumpOffTime >= 60000) { // Wait at least 10 seconds after the pump was last turned off
      controlPump(true);
      //controlRelay(true);
      pumpState = true;
      initialMoisture = substrateMoisture; // Store the initial moisture level
      Serial.print("Target Moisture: ");
      Serial.println(targetHumidity);
      Serial.print("Pump turned ON at humidity: ");
      Serial.println(substrateMoisture);

      // Get the current solution and substrate temperatures
      float solutionTemp, substrateTemp;
      getDS18B20Data(solutionTemp, substrateTemp);

      // Get the current atmospheric temperature and humidity
      float atmosTemp, atmosHumidity;
      getBME280Data(atmosTemp, atmosHumidity);

      // Log the data
      logPumpDataToSD(currentTime, solutionTemp, substrateTemp, atmosTemp, atmosHumidity, substrateMoisture, lightIntensity);
    } else if (pumpState && substrateMoisture >= initialMoisture + 2) { // Compare the current moisture level to the initial one
      controlPump(false);
      //controlRelay(false);
      pumpState = false;
      pumpOffTime = currentTime; // Save the time when the pump was turned off
      Serial.print("Target Moisture: ");
      Serial.println(targetHumidity);
      Serial.print("Pump turned OFF at humidity: ");
      Serial.println(substrateMoisture);
    }
  }
}

*/
/* v3 main code for updateWatering
void updateWatering(float atmosTemp, float substrateMoisture, float lightIntensity) {
  float targetHumidity;
  float hysteresis = 3.0; // Adjust as needed
  getSubstrateMoistureTarget(atmosTemp, lightIntensity, targetHumidity);
  static bool pumpState = false; // Keep track of the pump state

  if (millis() - startTime > 5000) { // Wait for 5 seconds after reboot
    // Existing pump control logic
    if (!pumpState && substrateMoisture < targetHumidity - hysteresis) {
      controlPump(true);
      controlRelay(true);
      pumpState = true;

      // Get the current time
      time_t timestamp = now();

      // Get the current solution and substrate temperatures
      float solutionTemp, substrateTemp;
      getDS18B20Data(solutionTemp, substrateTemp);
Serial.print("Solution Temp: ");
Serial.println(solutionTemp);
Serial.print("Substrate Temp: ");
Serial.println(substrateTemp);

      // Get the current atmospheric temperature and humidity
      float atmosTemp, atmosHumidity;
      getBME280Data(atmosTemp, atmosHumidity);
Serial.print("Atmospheric Temp: ");
Serial.println(atmosTemp);
Serial.print("Atmospheric Humidity: ");
Serial.println(atmosHumidity);

      // Log the data
      logPumpDataToSD(timestamp, solutionTemp, substrateTemp, atmosTemp, atmosHumidity, substrateMoisture, lightIntensity);
    } else if (pumpState && substrateMoisture > targetHumidity) {
      controlPump(false);
      controlRelay(false);
      pumpState = false;
    }
  }
}
*/
/* v2
void updateWatering(float atmosTemp, float substrateMoisture, float lightIntensity) {
  float targetHumidity;
  float hysteresis = 3.0; // Adjust as needed
  getSubstrateMoistureTarget(atmosTemp, lightIntensity, targetHumidity);
  static bool pumpState = false; // Keep track of the pump state

  if (millis() - startTime > 5000) { // Wait for 5 seconds after reboot
  // Existing pump control logic
  if (!pumpState && substrateMoisture < targetHumidity - hysteresis) {
    controlPump(true);
    controlRelay(true);
    pumpState = true;
  } else if (pumpState && substrateMoisture > targetHumidity) {
    controlPump(false);
    controlRelay(false);
    pumpState = false;
  }
}
}
*/

/* v1
void updateWatering(float atmosTemp, float substrateMoisture) {
  float targetHumidity;
  float hysteresis = 3.0; // Adjust as needed
  getSubstrateMoistureTarget(atmosTemp, targetHumidity);
  static bool pumpState = false; // Keep track of the pump state

  if (millis() - startTime > 5000) { // Wait for 5 seconds after reboot
  // Existing pump control logic
  if (!pumpState && substrateMoisture < targetHumidity - hysteresis) {
    controlPump(true);
    controlRelay(true);
    pumpState = true;
  } else if (pumpState && substrateMoisture > targetHumidity) {
    controlPump(false);
    controlRelay(false);
    pumpState = false;
  }
}
}
*/