#pragma once
#include <Arduino.h>
#include <PID_v1.h>
#include <IntervalTimer.h>

class TimingController {
public:
    static void initialize();
    static void setTiming(double percentage); // 0-100%
    static void update();
    static double getTimingDemand() { return timingDemand; }
    
private:
    // Smoothing variables
    static const int numReadings = 10;
    static int readings[numReadings];
    static int readIndex;
    static int total;
    static int average;
    
    // PID variables
    static double setpoint;        // Target percentage (0-100)
    static double input;           // Current sensor percentage
    static double output;          // PID output (0-20% for solenoid range)
    static double timingDemand;    // Current timing demand (future: degrees BTDC)
    static double Kp, Ki, Kd;     // PID gains
    
    // PID controller
    static PID* myPID;
    
    // PWM output
    static volatile int pwm_value;
    
    // PWM timer
    static IntervalTimer pwmTimer;
    
    static void updateSensor();
    static void updatePWM();
    
    friend void timingPWMUpdate(); // For IntervalTimer callback
};