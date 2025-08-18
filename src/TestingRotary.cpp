#include "TestingRotary.h"
#include "PCUPinMapping.h"

// Static member definitions
Adafruit_seesaw TestingRotary::ss;
int32_t TestingRotary::encoder_position[4] = {0, 0, 0, 0};
float* TestingRotary::setpoint_ptr = nullptr;
float* TestingRotary::kp_ptr = nullptr;
float* TestingRotary::ki_ptr = nullptr;
float* TestingRotary::kd_ptr = nullptr;
bool TestingRotary::last_button_state = true; // Pull-up, so true when not pressed
unsigned long TestingRotary::last_button_time = 0;

float r1Step = 10;

void TestingRotary::initialize() {
    
    // Initialize NeoRotary 4
    Serial.print("Trying to connect to seesaw at address 0x");
    Serial.println(SEESAW_ADDR, HEX);
    
    if (!ss.begin(SEESAW_ADDR)) {
        Serial.println("Couldn't find seesaw on default I2C bus");
        Serial.println("Check wiring and I2C address!");
        return;
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
    
    ss.enableEncoderInterrupt();
    
    // Configure encoder 0 button pin and initialize state
    ss.pinMode(12, INPUT_PULLUP); // Encoder 0 switch is on pin 12
    last_button_state = ss.digitalRead(12);
    last_button_time = millis();
    
    Serial.println("TestingRotary initialized");
}

void TestingRotary::setPointers(float* setpoint_ptr_arg, float* kp_ptr_arg, float* ki_ptr_arg, float* kd_ptr_arg) {
    setpoint_ptr = setpoint_ptr_arg;
    kp_ptr = kp_ptr_arg;
    ki_ptr = ki_ptr_arg;
    kd_ptr = kd_ptr_arg;
}

void TestingRotary::update() {
    // Read all rotary encoders when interrupt occurs
        for (int i = 0; i < 4; i++) {
            int32_t new_position = ss.getEncoderPosition(i);
            if (encoder_position[i] != new_position) {
                int32_t old_position = encoder_position[i];
                encoder_position[i] = new_position;
                
                if (i == 3 && setpoint_ptr) {
                    // Rotary 3: Set target percentage (0-100%) - step by 20
                    if (new_position > old_position) {
                        *setpoint_ptr += r1Step;  // Up
                    } else if (*setpoint_ptr - r1Step >= 0.0) {
                        *setpoint_ptr -= r1Step;  // Down, but don't go below 0
                    }
                    *setpoint_ptr = constrain(*setpoint_ptr, 0.0, 100.0);
                } else if (i == 2 && kp_ptr) {
                    // Rotary 2: Kp
                    if (new_position > old_position) {
                        *kp_ptr += 0.1;  // Up
                    } else if (*kp_ptr - 0.1 >= 0.0) {
                        *kp_ptr -= 0.1;  // Down, but don't go below 0
                    }
                    *kp_ptr = constrain(*kp_ptr, 0.0, 10.0);
                } else if (i == 1 && ki_ptr) {
                    // Rotary 1: Ki
                    if (new_position > old_position) {
                        *ki_ptr += 0.01;  // Up
                    } else if (*ki_ptr > 0.005) {  // Use 0.005 threshold instead of exact 0.0
                        *ki_ptr -= 0.01;  // Down, but don't go below ~0
                    }
                    *ki_ptr = constrain(*ki_ptr, 0.0, 10.0);
                } else if (i == 0 && kd_ptr) {
                    // Rotary 0: Kd
                    if (new_position > old_position) {
                        *kd_ptr += 0.001;  // Up
                    } else if (*kd_ptr > 0.005) {  // Use 0.005 threshold instead of exact 0.0
                        *kd_ptr -= 0.001;  // Down, but don't go below ~0
                    }
                    *kd_ptr = constrain(*kd_ptr, 0.0, 10.0);
                }
            }
    }
}

bool TestingRotary::checkButtonPressed() {
    bool current_button_state = ss.digitalRead(12); // Check encoder 0's button (pin 12)
    unsigned long current_time = millis();
    
    // Debounce: check if button state changed and enough time has passed
    if (current_button_state != last_button_state && (current_time - last_button_time) > 50) {
        last_button_state = current_button_state;
        last_button_time = current_time;
        
        // Return true on button press (transition from high to low due to pull-up)
        if (!current_button_state) {
            Serial.println("Encoder 0 button pressed - resetting PID!");
            return true;
        }
    }
    
    return false;
}