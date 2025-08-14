#pragma once
#include <Arduino.h>
#include <IntervalTimer.h>
#include "PCUPinMapping.h"
#include "TimingController.h"
#include "TestingRotary.h"

class PCU {
public:
    static void initialize();
    
    // Getters for TimingController access
    static double* getKpPtr() { return kp_ptr; }
    static double* getKiPtr() { return ki_ptr; }
    static double* getKdPtr() { return kd_ptr; }
    
private:
    // Global variables for rotary control
    static double setpoint;       // Target percentage (0-100%)
    static double Kp;             // Proportional gain
    static double Ki;             // Integral gain 
    static double Kd;             // Derivative gain
    
    // Global pointers for TimingController access
    static double* kp_ptr;
    static double* ki_ptr;
    static double* kd_ptr;
    
    // Timers
    static IntervalTimer mainTimer;
    static IntervalTimer debugTimer;
    
    // Static methods
    static void mainProcess();
    static void debugSerial();
};