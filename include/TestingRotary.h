#pragma once
#include <Arduino.h>
#include "Adafruit_seesaw.h"

class TestingRotary {
public:
    static void initialize();
    static void setPointers(float* setpoint_ptr, float* kp_ptr, float* ki_ptr, float* kd_ptr);
    static void update();
    static bool checkButtonPressed();
    
private:
    static Adafruit_seesaw ss;
    static int32_t encoder_position[4];
    static const int SEESAW_ADDR = 0x49;
    
    // Pointers to values we update
    static float* setpoint_ptr;
    static float* kp_ptr;
    static float* ki_ptr;
    static float* kd_ptr;
    
    // Button state tracking
    static bool last_button_state;
    static unsigned long last_button_time;
    
    static void scanI2C();
};