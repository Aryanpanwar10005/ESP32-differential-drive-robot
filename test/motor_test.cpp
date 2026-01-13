#include <Arduino.h>
#include "../src/config.h"
#include "../src/modules/motor_control.h"

// Standalone Motor Test v1.0 - No BLE/GPS/Network dependencies
#define TEST_CYCLES 3
#define TEST_DURATION 5000  // 5s per test
#define STOP_DURATION 2000  // 2s stop between tests
#define MOTOR_TEST_SPEED 100
#define TURN_TEST_SPEED 80

unsigned long testStartTime;
unsigned long testPhaseStart;
int currentCycle = 0;
int currentPhase = 0;  // 0=forward,1=stop,2=back,3=stop,4=left,5=stop,6=right,7=stop
bool testComplete = false;

void printMotorStatus() {
    Serial.printf("[MOTOR] L:%+4d R:%+4d | Cycle %d/%d\n", 
                  getCurrentLeftSpeed(), getCurrentRightSpeed(), 
                  currentCycle, TEST_CYCLES);
}

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(2000);  // Allow serial to stabilize
    
    Serial.println("\n=== Motor Test v1.0 - Standalone Mode ===");
    Serial.println("CRITICAL: LIFT MOTORS OFF GROUND OR REMOVE WHEELS!");
    Serial.println("Monitor motor temperature (<60C) & current draw");
    Serial.println("Testing TB6612FNG + DC Motors (GPIO 25-33,14,15)");
    Serial.println();
    
    motorInit();
    Serial.println("[MOTOR] Initialization complete");
    Serial.println("Test sequence: Forward->Stop->Back->Stop->Left->Stop->Right->Stop");
    Serial.println("3 full cycles, then permanent stop\n");
    
    testStartTime = millis();
    testPhaseStart = millis();
}

void loop() {
    unsigned long now = millis();
    
    if (testComplete) {
        delay(1000);
        return;
    }
    
    // Phase timing (non-blocking)
    if (now - testPhaseStart >= (currentPhase % 2 == 1 ? STOP_DURATION : TEST_DURATION)) {
        testPhaseStart = now;
        currentPhase++;
        
        if (currentPhase >= 8) {  // Complete cycle
            currentPhase = 0;
            currentCycle++;
            Serial.println("\nCycle complete. Pausing 3s...\n");
            delay(3000);
        }
        
        if (currentCycle >= TEST_CYCLES) {
            testComplete = true;
            stopMotors();
            printMotorStatus();
            Serial.println("\nALL TESTS COMPLETE - MOTORS STOPPED");
            Serial.println("Check: All motors moved smoothly? No overheating?");
            return;
        }
    }
    
    // Execute current phase
    switch (currentPhase % 8) {
        case 0: // Forward
            Serial.printf("[TEST] Forward %d - %ds\n", MOTOR_TEST_SPEED, TEST_DURATION/1000);
            moveForward(MOTOR_TEST_SPEED);
            break;
        case 1: // Stop
            Serial.printf("[TEST] Stop - %ds\n", STOP_DURATION/1000);
            stopMotors();
            break;
        case 2: // Backward
            Serial.printf("[TEST] Backward %d - %ds\n", MOTOR_TEST_SPEED, TEST_DURATION/1000);
            moveBackward(MOTOR_TEST_SPEED);
            break;
        case 3: // Stop
            Serial.printf("[TEST] Stop - %ds\n", STOP_DURATION/1000);
            stopMotors();
            break;
        case 4: // Left turn
            Serial.printf("[TEST] Left turn %d - %ds\n", TURN_TEST_SPEED, TEST_DURATION/1000);
            turnLeft(TURN_TEST_SPEED);
            break;
        case 5: // Stop
            Serial.printf("[TEST] Stop - %ds\n", STOP_DURATION/1000);
            stopMotors();
            break;
        case 6: // Right turn
            Serial.printf("[TEST] Right turn %d - %ds\n", TURN_TEST_SPEED, TEST_DURATION/1000);
            turnRight(TURN_TEST_SPEED);
            break;
        case 7: // Stop
            Serial.printf("[TEST] Stop - %ds\n", STOP_DURATION/1000);
            stopMotors();
            break;
    }
    
    printMotorStatus();
    delay(500);  // Status update rate
}
