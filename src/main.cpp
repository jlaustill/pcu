#include <Arduino.h>
#include "PCU.h"

void setup() {
  PCU::initialize();
}

void loop() {
  // Empty - all processing handled by PCU IntervalTimers
}