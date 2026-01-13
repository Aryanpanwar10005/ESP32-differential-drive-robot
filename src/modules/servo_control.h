#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include <Arduino.h>
#include <ESP32Servo.h>
#include "config.h"

class ServoControl {
private:
    Servo laserServo;
    uint8_t currentAngle;
    
public:
    void servoInit();
    void setLaserAngle(uint8_t degrees);
    uint8_t getLaserAngle();
};

extern ServoControl servoControl;

#endif // SERVO_CONTROL_H
