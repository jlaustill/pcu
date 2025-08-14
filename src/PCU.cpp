#include "PCU.h"

// Static member definitions
double PCU::setpoint = 0.0;
double PCU::Kp = 0.0;
double PCU::Ki = 0.0;
double PCU::Kd = 0.0;

double* PCU::kp_ptr = &PCU::Kp;
double* PCU::ki_ptr = &PCU::Ki;
double* PCU::kd_ptr = &PCU::Kd;

IntervalTimer PCU::mainTimer;
IntervalTimer PCU::debugTimer;

void PCU::initialize() {
    Serial.begin(115200);
    analogReadResolution(15);
    analogWriteResolution(15); // 32,768 steps  

    // Initialize classes
    TimingController::initialize();
    TestingRotary::initialize();
    TestingRotary::setPointers(&setpoint, &Kp, &Ki, &Kd);
    
    // Start main process timer (10ms = 10000 microseconds)
    mainTimer.begin(mainProcess, 10000);
    Serial.println("Main process timer started (10ms interval)");
    
    // Start debug serial timer (50ms = 50000 microseconds)
    debugTimer.begin(debugSerial, 50000);
    Serial.println("Debug serial timer started (50ms interval)");
    
    Serial.println("PCU initialized - all systems ready");
}

// Main process method called every 10ms
void PCU::mainProcess() {
    // Update rotary encoders
    TestingRotary::update();
    
    // Set timing based on rotary 0 setpoint
    TimingController::setTiming(setpoint);
    
    // Update timing controller
    TimingController::update();
}

// Debug serial method called every 50ms
void PCU::debugSerial() {
    double timingDemand = TimingController::getTimingDemand();
    
    Serial.print("Target: ");
    Serial.print(setpoint, 1);
    Serial.print("% | TimingDemand: ");
    Serial.print(timingDemand, 1);
    Serial.print("% | Kp: ");
    Serial.print(Kp, 2);
    Serial.print(" | Ki: ");
    Serial.print(Ki, 3);
    Serial.print(" | Kd: ");
    Serial.println(Kd, 3);
}