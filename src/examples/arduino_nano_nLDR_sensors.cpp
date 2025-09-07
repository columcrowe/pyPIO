#include <Arduino.h>

#define N_SENSORS 3
#define DELAYVALUE 1000

const uint8_t sensorPin[N_SENSORS] = {A0,A1,A2};
float baseline_env[N_SENSORS];
float baseline_loc[N_SENSORS];

void setup() {
	Serial.begin(115200);
	
	// Calibration
	for (uint8_t i=0;i<N_SENSORS;i++) {
		delay(DELAYVALUE*3);
		baseline_env[i] = (float)analogRead(sensorPin[i]); //"this is my normal"
		//Serial.println("Calibration complete. Baseline: " + String(baseline));
		baseline_loc[i] = baseline_env[i];
	}
}

// Read LDR sensor from analog pin. Return brightness value
uint8_t readSensor(uint8_t sensorID) {
	int sensorValue = analogRead(sensorPin[sensorID]);
	// Slowly track ambient changes, detect shadows
	if (sensorValue > baseline_loc[sensorID]) {
		// Light returning - update quickly
		baseline_loc[sensorID] = (baseline_env[sensorID] * 0.3 + sensorValue * 0.7);  // MOSTLY NEW
	} else {
		baseline_loc[sensorID] = (baseline_env[sensorID] * 0.999 + sensorValue * 0.001);  // 99% old, 1% new
	}
	int delta = (int)(baseline_loc[sensorID] - sensorValue); //more neg if env increase, more pos if env decrease
	int max_delta = max((int)(baseline_loc[sensorID]), 1); //normally diff from env (ambient light) to 0 pitch black (0)
	int brightness = map(delta, 0, max_delta, 0, 255); //map value from (min_delta, max_delta) to (min_brightness, max_brightness)
	brightness = constrain(brightness, 0, 255); //handle <(min_delta=0) deltas from mapping
	return (uint8_t)brightness;  // Cast to ensure uint8_t
}

void loop() {
	for (uint8_t i=0;i<N_SENSORS;i++) {
		uint8_t brightness_val = readSensor(i);
		Serial.print(brightness_val);
		if (i < N_SENSORS-1) Serial.print(",");
	}
	Serial.println();
	delay(DELAYVALUE/100);  // Delay to prevent overwhelming the processor	
}