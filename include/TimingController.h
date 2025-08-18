#pragma once
#include <Arduino.h>
#include <QuickPID.h>
#include <sTune.h>
#include "TimingSensor.h"
#include <AD7606p16_t4.h>

class TimingController {
public:
    static void initialize(AD7606p16_t4& adc);        
    static void updatePWM();    
    static void setDemandedTiming(float percentage); // 0-100%
    static void setPwmOutput();
    static float timingSensorPositionPercentage;           // Current sensor percentage
    static float timingSolenoidOutputPercentage;          // PID output (0-100% for solenoid range)
    
private:
    // PID variables
    static float timingDemandedSetpoint;        // Target percentage (0-100)
    static float Kp, Ki, Kd;     // PID gains
    
    // ADC instance reference
    static AD7606p16_t4* adc1;
    
    // PWM output and rate limiting
    static volatile int timingSolenoidOutputPwmValue;
    static float lastTimingPwmOutputValue;
    static float max_output_change;
};