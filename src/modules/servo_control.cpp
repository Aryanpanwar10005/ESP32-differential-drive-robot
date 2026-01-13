#include "servo_control.h"

ServoControl servoControl;

void ServoControl::servoInit() {
    laserServo.attach(SERVO_PIN);
    currentAngle = SERVO_CENTER_ANGLE;
    laserServo.write(currentAngle);
    Serial.println("[SERVO] Laser servo initialized on GPIO 13 (50Hz PWM, 10-170° safety limits)");
}

void ServoControl::setLaserAngle(uint8_t degrees) {
    // Safety constrain: 10-170 degrees only
    currentAngle = constrain(degrees, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
    laserServo.write(currentAngle);
    Serial.println("[SERVO] Angle: " + String(currentAngle));
}

uint8_t ServoControl::getLaserAngle() {
    return currentAngle;
}
