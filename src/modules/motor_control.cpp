#include "motor_control.h"
#include <Arduino.h>
#include <driver/ledc.h>

// Internal state
static bool motorsEnabled = false;
static int16_t currentLeftSpeed = 0;
static int16_t currentRightSpeed = 0;

void motorInit() {
    // Configure GPIO pins
    pinMode(LEFT_IN1, OUTPUT);
    pinMode(LEFT_IN2, OUTPUT);
    pinMode(RIGHT_IN1, OUTPUT);
    pinMode(RIGHT_IN2, OUTPUT);
    pinMode(MOTOR_STBY, OUTPUT);
    
    // Initial safe state: all LOW
    digitalWrite(LEFT_IN1, LOW);
    digitalWrite(LEFT_IN2, LOW);
    digitalWrite(RIGHT_IN1, LOW);
    digitalWrite(RIGHT_IN2, LOW);
    digitalWrite(MOTOR_STBY, LOW);
    
    // Configure LEDC PWM channels
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = (ledc_timer_bit_t)PWM_RES,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = PWM_FREQ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);
    
    ledc_channel_config_t left_channel = {
        .gpio_num = LEFT_PWM,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = (ledc_channel_t)PWM_CHANNEL_LEFT,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&left_channel);
    
    ledc_channel_config_t right_channel = {
        .gpio_num = RIGHT_PWM,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = (ledc_channel_t)PWM_CHANNEL_RIGHT,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&right_channel);
    
    // Enable motors (STBY HIGH)
    digitalWrite(MOTOR_STBY, HIGH);
    motorsEnabled = true;
    
    Serial.println("[MOTOR] Motor control initialized - STBY enabled");
}

void setMotorSpeeds(int16_t leftSpeed, int16_t rightSpeed) {
    if (!motorsEnabled) return;
    
    // Apply current limiting
    leftSpeed = constrain(leftSpeed, -MAX_SPEED, MAX_SPEED);
    rightSpeed = constrain(rightSpeed, -MAX_SPEED, MAX_SPEED);
    
    // Store current speeds
    currentLeftSpeed = leftSpeed;
    currentRightSpeed = rightSpeed;
    
    // Left motor control
    if (leftSpeed > 0) {
        // Forward
        digitalWrite(LEFT_IN1, HIGH);
        digitalWrite(LEFT_IN2, LOW);
        ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)PWM_CHANNEL_LEFT, leftSpeed);
    } else if (leftSpeed < 0) {
        // Reverse
        digitalWrite(LEFT_IN1, LOW);
        digitalWrite(LEFT_IN2, HIGH);
        ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)PWM_CHANNEL_LEFT, -leftSpeed);
    } else {
        // Stop
        digitalWrite(LEFT_IN1, LOW);
        digitalWrite(LEFT_IN2, LOW);
        ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)PWM_CHANNEL_LEFT, 0);
    }
    ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)PWM_CHANNEL_LEFT);
    
    // Right motor control
    if (rightSpeed > 0) {
        // Forward
        digitalWrite(RIGHT_IN1, HIGH);
        digitalWrite(RIGHT_IN2, LOW);
        ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)PWM_CHANNEL_RIGHT, rightSpeed);
    } else if (rightSpeed < 0) {
        // Reverse
        digitalWrite(RIGHT_IN1, LOW);
        digitalWrite(RIGHT_IN2, HIGH);
        ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)PWM_CHANNEL_RIGHT, -rightSpeed);
    } else {
        // Stop
        digitalWrite(RIGHT_IN1, LOW);
        digitalWrite(RIGHT_IN2, LOW);
        ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)PWM_CHANNEL_RIGHT, 0);
    }
    ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)PWM_CHANNEL_RIGHT);
    
    Serial.printf("[MOTOR] L=%d, R=%d\n", leftSpeed, rightSpeed);
}

void stopMotors() {
    setMotorSpeeds(0, 0);
    Serial.println("[MOTOR] Motors stopped");
}

void emergencyStop() {
    motorsEnabled = false;
    currentLeftSpeed = 0;
    currentRightSpeed = 0;
    digitalWrite(MOTOR_STBY, LOW);
    digitalWrite(LEFT_IN1, LOW);
    digitalWrite(LEFT_IN2, LOW);
    digitalWrite(RIGHT_IN1, LOW);
    digitalWrite(RIGHT_IN2, LOW);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)PWM_CHANNEL_LEFT, 0);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)PWM_CHANNEL_RIGHT, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)PWM_CHANNEL_LEFT);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)PWM_CHANNEL_RIGHT);
    Serial.println("[MOTOR] EMERGENCY STOP - All motors disabled!");
}

void moveForward(int16_t speed) {
    setMotorSpeeds(speed, speed);
}

void moveBackward(int16_t speed) {
    setMotorSpeeds(-speed, -speed);
}

void turnLeft(int16_t speed) {
    setMotorSpeeds(-speed, speed);
}

void turnRight(int16_t speed) {
    setMotorSpeeds(speed, -speed);
}

// Speed getters for testing
int getCurrentLeftSpeed() {
    return currentLeftSpeed;
}

int getCurrentRightSpeed() {
    return currentRightSpeed;
}
