#include <ESP8266WiFi.h>
#include "ESPAsyncUDP.h"

#define DELAYVALUE 1000

// WiFi hotspot creds
const char* ssid = "Galaxy A01 Core8885";
const char* password = "randompass123";
// Arduino server deets (remote)
IPAddress serverIP(192,168,43,100); // static IP in hotspot range
const uint16_t serverPort = 12345;
// Hardware setup
const uint8_t sensorPin = 17;	// A0
const uint8_t sensorID = 1;     // Change ID for each Sensor/ESP8266 combo (1, 2, 3, etc.)
// Static IP configuration for mobile hotspot
IPAddress local_IP(192, 168, 43, 100+sensorID);    // static IP (in hotspot range)
IPAddress gateway(192, 168, 43, 56);      		   // hotspot's gateway
IPAddress subnet(255, 255, 255, 0);       		   // Standard subnet mask

AsyncUDP udp;
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

    if(udp.connect(serverIP, serverPort)) {
/*         Serial.println("UDP connected");
        udp.onPacket([](AsyncUDPPacket packet) {
            Serial.print("UDP Packet Type: ");
            Serial.print(packet.isBroadcast()?"Broadcast":packet.isMulticast()?"Multicast":"Unicast");
            Serial.print(", From: ");
            Serial.print(packet.remoteIP());
            Serial.print(":");
            Serial.print(packet.remotePort());
            Serial.print(", To: ");
            Serial.print(packet.localIP());
            Serial.print(":");
            Serial.print(packet.localPort());
            Serial.print(", Length: ");
            Serial.print(packet.length());
            Serial.print(", Data: ");
            Serial.write(packet.data(), packet.length());
            Serial.println();
            //reply to the client
            packet.printf("Got %u bytes of data", packet.length());
        });
        //Send unicast
        udp.print("Hello Server!"); */
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
	uint8_t buffer[2] = {sensorID, brightness_val};
	udp.broadcastTo(buffer, 2, serverPort);

	delay(DELAYVALUE/100);  // Small delay to prevent overwhelming the processor
}