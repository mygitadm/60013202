#ifndef PUMP_H
#define PUMP_H

#include <Arduino.h>



//=========L293D=========//
extern unsigned long lastPumpOnTime;
extern const unsigned long pumpOnInterval;
extern const int MOTOR_SPEED_PIN;
extern const int MOTOR_CONTROL_PIN1;
extern const int MOTOR_CONTROL_PIN2;

//=========relay=======//
#define relayPin 48

void setupPump();
void updateWatering(float atmosTemp, float substrateMoisture);




#endif // PUMP_H