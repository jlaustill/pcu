#include "TestingRotary.h"
#include "PCUPinMapping.h"
#include <Wire.h>

// Static member definitions
Adafruit_seesaw TestingRotary::ss;
int32_t TestingRotary::encoder_position[4] = {0, 0, 0, 0};
double* TestingRotary::setpoint_ptr = nullptr;
double* TestingRotary::kp_ptr = nullptr;
double* TestingRotary::ki_ptr = nullptr;
double* TestingRotary::kd_ptr = nullptr;

void TestingRotary::initialize() {
    // Initialize I2C and scan for devices
    Wire.begin();
    delay(1000); // Give devices time to initialize
    scanI2C();
    
    // Set up interrupt pin
    pinMode(ROTARY_IRQ_PIN, INPUT_PULLUP);
    
    // Initialize NeoRotary 4
    Serial.print("Trying to connect to seesaw at address 0x");
    Serial.println(SEESAW_ADDR, HEX);
    
    if (!ss.begin(SEESAW_ADDR)) {
        Serial.println("Couldn't find seesaw on default I2C bus");
        Serial.println("Check wiring and I2C address!");
        while (1) delay(10);
    }
    
    Serial.println("Seesaw started");
    
    uint32_t version = ((ss.getVersion() >> 16) & 0xFFFF);
    Serial.print("Found seesaw firmware version: ");
    Serial.println(version);
    
    if (version == 4991) {
        Serial.println("Found Product 4991 (NeoRotary)");
    } else if (version == 5752) {
        Serial.println("Found Product 5752 (Compatible seesaw device)");
    } else {
        Serial.print("Unknown firmware version: ");
        Serial.println(version);
        Serial.println("Continuing anyway...");
    }
    
    // Get starting positions for all encoders
    for (int i = 0; i < 4; i++) {
        encoder_position[i] = ss.getEncoderPosition(i);
    }
    
    ss.pinMode(SS_SWITCH, INPUT_PULLUP);
    ss.setGPIOInterrupts((uint32_t)1 << SS_SWITCH, 1);
    ss.enableEncoderInterrupt();
    
    Serial.println("TestingRotary initialized");
}

void TestingRotary::setPointers(double* setpoint_ptr_arg, double* kp_ptr_arg, double* ki_ptr_arg, double* kd_ptr_arg) {
    setpoint_ptr = setpoint_ptr_arg;
    kp_ptr = kp_ptr_arg;
    ki_ptr = ki_ptr_arg;
    kd_ptr = kd_ptr_arg;
}

void TestingRotary::update() {
    // Read all rotary encoders when interrupt occurs
    if (!digitalRead(ROTARY_IRQ_PIN)) {
        for (int i = 0; i < 4; i++) {
            int32_t new_position = ss.getEncoderPosition(i);
            if (encoder_position[i] != new_position) {
                int32_t old_position = encoder_position[i];
                encoder_position[i] = new_position;
                
                if (i == 0 && setpoint_ptr) {
                    // Rotary 0: Set target percentage (0-100%)
                    if (new_position > old_position) {
                        *setpoint_ptr += 2.5;  // Up
                    } else if (*setpoint_ptr - 2.5 >= 0.0) {
                        *setpoint_ptr -= 2.5;  // Down, but don't go below 0
                    }
                    *setpoint_ptr = constrain(*setpoint_ptr, 0.0, 100.0);
                } else if (i == 1 && kp_ptr) {
                    // Rotary 1: Kp
                    if (new_position > old_position) {
                        *kp_ptr += 0.1;  // Up
                    } else if (*kp_ptr - 0.1 >= 0.0) {
                        *kp_ptr -= 0.1;  // Down, but don't go below 0
                    }
                    *kp_ptr = constrain(*kp_ptr, 0.0, 10.0);
                } else if (i == 2 && ki_ptr) {
                    // Rotary 2: Ki
                    if (new_position > old_position) {
                        *ki_ptr += 0.01;  // Up
                    } else if (*ki_ptr > 0.005) {  // Use 0.005 threshold instead of exact 0.0
                        *ki_ptr -= 0.01;  // Down, but don't go below ~0
                    }
                    *ki_ptr = constrain(*ki_ptr, 0.0, 1.0);
                } else if (i == 3 && kd_ptr) {
                    // Rotary 3: Kd
                    if (new_position > old_position) {
                        *kd_ptr += 0.01;  // Up
                    } else if (*kd_ptr > 0.005) {  // Use 0.005 threshold instead of exact 0.0
                        *kd_ptr -= 0.01;  // Down, but don't go below ~0
                    }
                    *kd_ptr = constrain(*kd_ptr, 0.0, 1.0);
                }
            }
        }
    }
}

void TestingRotary::scanI2C() {
    Serial.println("Scanning I2C bus...");
    byte count = 0;
    
    for (byte address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0) {
            Serial.print("Found I2C device at address 0x");
            if (address < 16) Serial.print("0");
            Serial.println(address, HEX);
            count++;
        }
    }
    
    if (count == 0) {
        Serial.println("No I2C devices found!");
    } else {
        Serial.print("Found ");
        Serial.print(count);
        Serial.println(" I2C device(s).");
    }
    Serial.println();
}