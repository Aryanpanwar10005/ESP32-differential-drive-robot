#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include "../config.h"
#include <Arduino.h>
#include <ESP32Servo.h>

class ServoControl {
private:
  Servo laserServo;
  uint8_t currentAngle;

public:
  // Constructor to initialize member variables
  ServoControl() : currentAngle(SERVO_CENTER_ANGLE) {}

  void servoInit();
  void setLaserAngle(uint8_t degrees);
  uint8_t getLaserAngle();
};

extern ServoControl servoControl;

#endif // SERVO_CONTROL_H
