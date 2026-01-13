#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "../config.h"
#include <Arduino.h>

void motorInit();
void setMotorSpeeds(int16_t leftSpeed, int16_t rightSpeed);
void stopMotors();
void emergencyStop();
void moveForward(int16_t speed);
void moveBackward(int16_t speed);
void turnLeft(int16_t speed);
void turnRight(int16_t speed);

// Speed getters for testing
int getCurrentLeftSpeed();
int getCurrentRightSpeed();

#endif // MOTOR_CONTROL_H
