#pragma once
#include <Arduino.h>
#include "Adafruit_seesaw.h"

class TestingRotary {
public:
    static void initialize();
    static void setPointers(double* setpoint_ptr, double* kp_ptr, double* ki_ptr, double* kd_ptr);
    static void update();
    
private:
    static Adafruit_seesaw ss;
    static int32_t encoder_position[4];
    static const int SS_SWITCH = 24;
    static const int SEESAW_ADDR = 0x49;
    
    // Pointers to values we update
    static double* setpoint_ptr;
    static double* kp_ptr;
    static double* ki_ptr;
    static double* kd_ptr;
    
    static void scanI2C();
};