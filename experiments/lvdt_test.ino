#include <Audio.h>

// Audio system setup
AudioSynthWaveform       sine1;
AudioOutputI2S           i2s1;
AudioConnection          patchCord1(sine1, 0, i2s1, 0);  // left channel
AudioConnection          patchCord2(sine1, 0, i2s1, 1);  // right channel


AudioInputAnalog audioInput(A0);  // Use pin A0 for LVDT signal
AudioAnalyzePeak peak1;
AudioConnection patchCord3(audioInput, 0, peak1, 0);

float filteredPeak = 0;
const float FILTER_ALPHA = 0.1; // Lower = more filtering


void setup() {
  Serial.begin(115200);
  AudioMemory(16);

  sine1.begin(WAVEFORM_SINE);
  sine1.frequency(10000);    // test with 1kHz
  sine1.amplitude(1.0);     // full-scale = 1.0
}

void loop() {
  if (peak1.available()) {
    float currentPeak = peak1.read() * 1000;
    
    // Simple exponential moving average
    filteredPeak = (FILTER_ALPHA * currentPeak) + ((1.0 - FILTER_ALPHA) * filteredPeak);
    
    // Serial.print("Filtered Peak: ");
    // Serial.print(filteredPeak);

    float percent = map(filteredPeak, 100, 465, 0 , 1000);
    percent = percent > 1000 ? 1000 : percent;
    percent = percent < 0 ? 0 : percent;
    Serial.print(" as %: ");
    Serial.print(percent / 10);
    Serial.println("");
  }
}
