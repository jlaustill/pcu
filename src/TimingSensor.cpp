#include "TimingSensor.h"
#include "PCUPinMapping.h"


// Static member definitions
SeaDash::Uint32::IncrementalCircularAverage TimingSensor::averager(10);
AD7606p16_t4* TimingSensor::adc1 = nullptr;
float TimingSensor::currentVoltage = 0;


void TimingSensor::initialize(AD7606p16_t4& adc) {
    adc1 = &adc;
}

float TimingSensor::getCurrentReadingPercentage() {
    currentVoltage = adc1->getVoltage(0);
    currentVoltage = constrain(currentVoltage, TIMING_SENSOR_MIN, TIMING_SENSOR_MAX);
    
    // Map averaged reading to 0-100% based on calibration values
    return map(currentVoltage, TIMING_SENSOR_MIN, TIMING_SENSOR_MAX, 0, 100);;
}