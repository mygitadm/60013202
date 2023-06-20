#include "pump.h"
#include "sensors.h"



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


/*
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