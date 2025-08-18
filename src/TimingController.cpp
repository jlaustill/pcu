#include "TimingController.h"
#include "PCUPinMapping.h"
#include "PCU.h"

uint16_t refreshRate = 10000; // 10ms - better for PID timing

// Static member definitions
float TimingController::timingDemandedSetpoint = 0.0;
float TimingController::timingSensorPositionPercentage = 0.0;
float TimingController::timingSolenoidOutputPercentage = 0.0;
float TimingController::Kp = 0.0;
float TimingController::Ki = 0.0;
float TimingController::Kd = 0.0;
AD7606p16_t4* TimingController::adc1 = nullptr;

volatile int TimingController::timingSolenoidOutputPwmValue = 0;
float TimingController::lastTimingPwmOutputValue = 0.0;
float TimingController::max_output_change = 10.0; // Max 10% change per cycle (10ms)


void TimingController::initialize(AD7606p16_t4& adc) {
    adc1 = &adc;
    // Initialize hardware pins
    pinMode(TIMING_PWM_PIN, OUTPUT);    
    
    // Set motor direction and initial state
    analogWrite(TIMING_PWM_PIN, 0);
    
    Serial.println("TimingController initialized");
}

void TimingController::setDemandedTiming(float percentage) {
    timingDemandedSetpoint = constrain(percentage, 0.0, 100.0);
}

void TimingController::setPwmOutput() {    
        // Convert PID output (0-100%) to PWM value (0-4095 for 12-bit)
        timingSolenoidOutputPwmValue = map(timingDemandedSetpoint, 0, 100, 300, 3000);

        // Set 12-bit resolution and write PWM
        analogWriteResolution(12);
        analogWrite(TIMING_PWM_PIN, timingSolenoidOutputPwmValue);  

}


void TimingController::updatePWM() {
        // Update sensor input from TimingSensor
        timingSensorPositionPercentage = TimingSensor::getCurrentReadingPercentage();

        setPwmOutput();

        lastTimingPwmOutputValue = timingSolenoidOutputPercentage;
}