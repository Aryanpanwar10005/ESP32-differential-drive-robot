#include <Arduino.h>
#include "../src/config.h"
#include "../src/modules/gps_module.h"

unsigned long lastStatusPrint = 0;
unsigned long lastFullPrint = 0;
unsigned long noDataTimer = 0;
bool gpsDataReceived = false;
uint32_t sentenceCount = 0;

void printGPSStatus() {
    GPSData data = gpsModule.getGPSData();
    Serial.printf("[GPS] Chars: %lu | Fix: %s | Satellites: %d\n", 
                  gpsModule.getCharsProcessed(), 
                  data.fix_valid ? "YES" : "NO", 
                  gpsModule.getSatellites());
}

void printFullGPS() {
    GPSData data = gpsModule.getGPSData();
    Serial.printf("\nFULL GPS DATA (%.2fs uptime):\n", millis() / 1000.0);
    Serial.printf("  Lat: %.6f | Lon: %.6f\n", data.lat, data.lon);
    Serial.printf("  Fix: %s | Satellites: %d | HDOP: %.1f\n", 
                  data.fix_valid ? "YES" : "NO", 
                  gpsModule.getSatellites(),
                  gpsModule.getHDOP());
    Serial.printf("  Age: %lu ms | JSON: %s\n", 
                  data.timestamp, gpsModule.getGPSJSON().c_str());
    
    if (!data.fix_valid) {
        Serial.println("  TIP: Move outdoors or near window. Cold start: 30-60s");
    }
    Serial.println();
}

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(2000);
    
    Serial.println("\n=== GPS Test v1.0 - Standalone Mode ===");
    Serial.println("Testing NEO-6M GPS (GPIO 16 RX, 17 TX, 9600 baud)");
    Serial.println("Requires outdoor location or window view");
    Serial.println("Cold start typical: 30-60 seconds");
    Serial.println();
    
    gpsModule.gpsInit();
    Serial.println("[GPS] UART configured");
    
    lastStatusPrint = millis();
    lastFullPrint = millis();
    noDataTimer = millis();
}

void loop() {
    gpsModule.gpsUpdate();
    
    unsigned long now = millis();
    
    // Count sentences using getter
    uint32_t currentChars = gpsModule.getCharsProcessed();
    if (currentChars > sentenceCount) {
        sentenceCount = currentChars;
        gpsDataReceived = true;
        noDataTimer = now;
    }
    
    // Status every 1s
    if (now - lastStatusPrint >= 1000) {
        printGPSStatus();
        lastStatusPrint = now;
    }
    
    // Full data every 5s
    if (now - lastFullPrint >= 5000) {
        printFullGPS();
        lastFullPrint = now;
    }
    
    // No data warning
    if (gpsDataReceived && (now - noDataTimer > 30000)) {
        Serial.println("WARNING: NO GPS DATA >30s - Check wiring/power/antenna!");
        noDataTimer = now;
    }
    
    delay(100);
}
