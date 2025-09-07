#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

#define DELAYVALUE 1000

// WiFi hotspot creds
const char* ssid = "Galaxy A01 Core8885";
const char* password = "randompass123";
// Arduino server deets (remote)
const char* serverIP = "192.168.43.100"; // static IP in hotspot range
const uint16_t serverPort = 12345;
// Hardware setup
const uint8_t sensorPin = 17;	// A0
const uint8_t sensorID = 1;     // Change ID for each Sensor/ESP8266 combo (1, 2, 3, etc.)
// Static IP configuration for mobile hotspot
IPAddress local_IP(192, 168, 43, 100+sensorID);    // static IP (in hotspot range)
IPAddress gateway(192, 168, 43, 56);      		   // hotspot's gateway
IPAddress subnet(255, 255, 255, 0);       		   // Standard subnet mask

WiFiUDP udp;
float baseline_float = 0;
uint16_t baseline = 0;

void setup() {
	Serial.begin(9600); //FeatherHuzzah likes 9600 but not 115200 for some reason
  
	// Calibration
	delay(DELAYVALUE);
	baseline_float = analogRead(sensorPin); //"this is my normal"
	//Serial.println("Calibration complete. Baseline: " + String(baseline));
  
	// Connect to WiFi
	WiFi.config(local_IP, gateway, subnet); // Configure static IP
	WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("WiFi Failed");
        while(1) {
			delay(DELAYVALUE);
			Serial.print(".");
        }
    }
    Serial.println("\nWiFi connected!");
    Serial.println("IP address: " + WiFi.localIP().toString());  
  
    // Start UDP listening server - echo sensor reading
    if (udp.begin(serverPort)) { // start listening on udpPort
	  Serial.print("UDP listening on port ");
	  Serial.println(serverPort);
    } else {
      Serial.println("Failed to start");
    }
}

void loop() {
	// Read sensor
	int sensorValue = analogRead(sensorPin);
	// Slowly track ambient changes, detect shadows
	if (sensorValue > baseline) {
		// Light returning - update quickly
		baseline = (uint16_t)(baseline_float * 0.3 + sensorValue * 0.7);  // MOSTLY NEW
	} else {
		baseline = (uint16_t)(baseline_float * 0.999 + sensorValue * 0.001);  // 99% old, 1% new
	}
	int delta = baseline - sensorValue; //more neg if env increase, more pos if env decrease
	int max_delta = baseline; //normally diff from env (ambient light) to 0 pitch black (0)
	int brightness = map(delta, 0, max_delta, 0, 255); //map value from (min_delta, max_delta) to (min_brightness, max_brightness)
	brightness = constrain(brightness, 0, 255); //handle <(min_delta=0) deltas from mapping
	uint8_t brightness_val = (uint8_t)brightness;  // Cast to ensure uint8_t
	
	String data = String(sensorID) + "," + String(brightness);
	//Serial.println("Sent: " + data);
	Serial.println("Sensor " + String(sensorID) + " brightness: " + String(brightness));
  
	// Send data --- UDP packet (sensorID,brightness) ---
	udp.beginPacket(serverIP, serverPort);
	udp.write(&sensorID, 1);       // Send 1 byte
	udp.write(&brightness_val, 1); // Send 1 byte
	udp.endPacket();
	
	delay(DELAYVALUE/100);  // Small delay to prevent overwhelming the processor
}