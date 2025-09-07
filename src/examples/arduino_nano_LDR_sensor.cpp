#include <Arduino.h>

#define DELAYVALUE 1000

const uint8_t sensorPin = A0;
float baseline_env = 0;
float baseline_loc = 0;

void setup() {
	Serial.begin(115200);
  
	// Calibration
	delay(DELAYVALUE);
	baseline_env = (float)(analogRead(sensorPin)); //"this is my normal"
	//Serial.println("Calibration complete. Baseline: " + String(baseline));
	baseline_loc = baseline_env;
	
}

void loop() {
	// Read sensor
	int sensorValue = analogRead(sensorPin);
	// Slowly track ambient changes, detect shadows
	if (sensorValue > baseline_loc) {
		// Light returning - update quickly
		baseline_loc = (baseline_env * 0.3 + sensorValue * 0.7);  // MOSTLY NEW
	} else {
		baseline_loc = (baseline_env * 0.999 + sensorValue * 0.001);  // 99% old, 1% new
	}	
	int delta = (int)(baseline_loc - sensorValue); //more neg if env increase, more pos if env decrease
	int max_delta = max((int)(baseline_loc), 1); //normally diff from env (ambient light) to 0 pitch black (0)
	int brightness = map(delta, 0, max_delta, 0, 255); //map value from (min_delta, max_delta) to (min_brightness, max_brightness)
	brightness = constrain(brightness, 0, 255); //handle <(min_delta=0) deltas from mapping
	uint8_t brightness_val = (uint8_t)brightness;  // Cast to ensure uint8_t
	
	Serial.print(brightness_val);
	//String data = String(brightness);
	//Serial.println(data);
	delay(DELAYVALUE/100);  // Delay to prevent overwhelming the processor
}