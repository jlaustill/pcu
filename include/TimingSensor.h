#pragma once
#include <Arduino.h>
#include <Uint32/IncrementalCircularAverage.hpp>
#include <AD7606p16_t4.h>

class TimingSensor {
public:
    static void initialize(AD7606p16_t4& adc);
    static float getCurrentReadingPercentage(); // Returns 0-100%
    
private:
    static SeaDash::Uint32::IncrementalCircularAverage averager;
    static AD7606p16_t4* adc1;
    static float currentVoltage;
};