#include "PCU.h"
#include "TimingSensor.h"

// Static member definitions
float PCU::setpoint = 0.0;
float PCU::Kp = 2.0;
float PCU::Ki = 1.0;
float PCU::Kd = 0.00;

AD7606p16_t4 PCU::adc1(RD_PIN, CS_PIN, CONV_START_PIN, BUSY_PIN, RESET_PIN, 20.0f);

uint16_t pwm_duty = 0;


void PCU::initialize() {
    // Initialize classes
    TimingController::initialize(adc1);
    TestingRotary::initialize();
    TestingRotary::setPointers(&setpoint, &Kp, &Ki, &Kd);

    analogWriteFrequency(TIMING_PWM_PIN, 20000);
    analogReadResolution(12);
    analogWrite(TIMING_PWM_PIN, pwm_duty);
    
    Serial.println("PCU initialized - all systems ready");
    Serial.println("*** CALIBRATION MODE ACTIVE ***");
    Serial.println("Use rotary0 to control PWM directly and observe raw sensor values");
    Serial.println("Call PCU::setCalibrationMode(false) to switch to normal PID mode");
}

// Main process method called every 10ms
void PCU::mainProcess() {
        // Update rotary encoder inputs
        TestingRotary::update();
        TimingController::setDemandedTiming(setpoint);
        TimingController::updatePWM();
}

// Debug serial method called every 50ms
void PCU::debugProcess() {  
        
        Serial.print("SetPoint: ");
        Serial.print(setpoint, 1);
        Serial.print("% | Sensor: ");
        Serial.print(TimingController::timingSensorPositionPercentage, 3);
        Serial.println();
}