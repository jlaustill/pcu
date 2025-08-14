#include "TimingController.h"
#include "PCUPinMapping.h"
#include "PCU.h"

// Static member definitions
int TimingController::readings[TimingController::numReadings];
int TimingController::readIndex = 0;
int TimingController::total = 0;
int TimingController::average = 0;

double TimingController::setpoint = 0.0;
double TimingController::input = 0.0;
double TimingController::output = 0.0;
double TimingController::timingDemand = 0.0;
double TimingController::Kp = 0.0;
double TimingController::Ki = 0.0;
double TimingController::Kd = 0.0;

PID* TimingController::myPID = nullptr;
volatile int TimingController::pwm_value = 0;
IntervalTimer TimingController::pwmTimer;

// Timer callback function
void timingPWMUpdate() {
    TimingController::updatePWM();
}

void TimingController::initialize() {
    // Initialize hardware pins
    pinMode(TIMING_INA_PIN, OUTPUT);
    pinMode(TIMING_INB_PIN, OUTPUT);
    pinMode(TIMING_PWM_PIN, OUTPUT);
    pinMode(TIMING_EN_PIN, OUTPUT);
    
    // Configure PWM
    analogWriteFrequency(TIMING_PWM_PIN, 4000); // 4 kHz
    
    // Set motor direction and initial state
    digitalWrite(TIMING_INA_PIN, HIGH);
    digitalWrite(TIMING_INB_PIN, LOW);
    analogWrite(TIMING_PWM_PIN, 0);
    digitalWrite(TIMING_EN_PIN, HIGH);
    
    // Initialize readings array
    for (int i = 0; i < numReadings; i++) {
        readings[i] = 0;
    }
    
    // Initialize PID controller
    myPID = new PID(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);
    myPID->SetOutputLimits(0.0, 20.0);   // PID output 0-20% (match solenoid range)
    myPID->SetSampleTime(10);             // 10ms sample time
    myPID->SetMode(AUTOMATIC);            // Start PID
    
    // Start PWM update timer (10ms = 10000 microseconds)
    pwmTimer.begin(timingPWMUpdate, 10000);
    Serial.println("PWM timer started (10ms interval)");
    
    Serial.println("TimingController initialized");
}

void TimingController::setTiming(double percentage) {
    setpoint = constrain(percentage, 0.0, 100.0);
    // Map to solenoid range (0-20%)
    setpoint = map(setpoint, 0.0, 100.0, 0.0, 20.0);
}

void TimingController::update() {
    updateSensor();
    
    // Update PID gains from PCU pointers
    double* kp_ptr = PCU::getKpPtr();
    double* ki_ptr = PCU::getKiPtr();
    double* kd_ptr = PCU::getKdPtr();
    
    if (kp_ptr && ki_ptr && kd_ptr) {
        if (Kp != *kp_ptr || Ki != *ki_ptr || Kd != *kd_ptr) {
            Kp = *kp_ptr;
            Ki = *ki_ptr;
            Kd = *kd_ptr;
            myPID->SetTunings(Kp, Ki, Kd);
        }
    }
    
    // Update timing demand (currently same as setpoint, future: convert to degrees BTDC)
    timingDemand = map(setpoint, 0.0, 20.0, 0.0, 100.0); // Convert back to 0-100% for now
}

void TimingController::updateSensor() {
    // Update sensor reading with smoothing
    total = total - readings[readIndex];
    readings[readIndex] = analogRead(TIMING_SENSOR_PIN);
    total = total + readings[readIndex];
    readIndex = readIndex + 1;
    if (readIndex >= numReadings) {
        readIndex = 0;
    }
    average = total / numReadings;
    
    // Update PID input (current sensor percentage)
    input = (average / 32767.0) * 100.0;
}

void TimingController::updatePWM() {
    // Update PID controller
    myPID->Compute();
    
    // Convert PID output (0-20%) to PWM value (0-32767)
    // 20% = 32767, so multiply by 1638.35
    pwm_value = (int)(output * 1638.35);
    
    // Apply PWM
    analogWrite(TIMING_PWM_PIN, pwm_value);
}