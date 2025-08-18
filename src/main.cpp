#include <Arduino.h>
#include "PCU.h"

elapsedMillis loopTimer;
elapsedMillis debugTimer;


void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10); // Wait for serial connection
    
  PCU::initialize();
}

void loop() {
  if (loopTimer >= 10) {    
    loopTimer = 0;  
    PCU::mainProcess();
  }

  if (debugTimer >= 500) {    
    debugTimer = 0;  
    PCU::debugProcess();
  }
}