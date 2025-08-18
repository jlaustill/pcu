#pragma once
#include <Arduino.h>
#include <AD7606p16_t4.h>
#include "PCUPinMapping.h"
#include "TimingController.h"
#include "TestingRotary.h"

class PCU {
public:
    static void initialize();
    
    // Static methods
    static void debugProcess();
    static void mainProcess();
    
    static float Kp;             // Proportional gain
    static float Ki;             // Integral gain 
    static float Kd;             // Derivative gain
    
private:
    // Global variable for ADC Voltages
    static float adcVoltages[8];
    static AD7606p16_t4 adc1;
    // Global variables for rotary control
    static float setpoint;       // Target percentage (0-100%)
};