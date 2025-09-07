//#include <Adafruit_NeoPixel.h>
#include <FastLED.h>
#include <WiFiS3.h>
#include <WiFiUDP.h>

#define N_CLIENTS 2
#define DELAYVALUE 1000
#define PIN 3 // for single data line (single-wire digital / WS2812 protocol)
#define N_STRIPS 5
#define N_LEDS 60
#define LED_COUNT N_LEDS*N_STRIPS
#define LEDS_PER_ZONE 24
CRGB pixels[LED_COUNT];

// WiFi creds
const char* ssid = "Galaxy A01 Core8885";
const char* password = "randompass123";
// Static IP configuration for mobile hotspot
IPAddress local_IP(192, 168, 43, 100);    // Arduino's fixed IP (in hotspot range)
IPAddress gateway(192, 168, 43, 56);      // hotspot's gateway
IPAddress subnet(255, 255, 255, 0);       // Standard subnet mask
const uint16_t localPort = 12345;
WiFiUDP udp;

enum clientIDs {
    client1ID = 1,
    client2ID = 2,
};
const char* clientIPs[N_CLIENTS] = {"192.168.43.101","192.168.43.102"};  // static IP in hotspot range
// LED indices for each zone (one per client)
const uint16_t ledIdx[N_CLIENTS][LEDS_PER_ZONE] = {
    [client1ID-1] = {0,1,2,3,4,5,6,7,8,9,10,11,108,109,110,111,112,113,114,115,116,117,118,119},
    [client2ID-1] = {180,181,182,183,184,185,186,187,188,189,190,191,289,290,291,292,293,294,295,296,297,298,299,300}
};
uint8_t sensorID, brightness;
uint16_t ledIndex;

void setup() {
  Serial.begin(115200);

  // FastLED setup
  FastLED.addLeds<WS2812, PIN, GRB>(pixels, LED_COUNT);
  delay(DELAYVALUE/1);
  FastLED.setBrightness(255);
  FastLED.clear();
  FastLED.show();

  // Configure static IP
  WiFi.config(local_IP, gateway, subnet);
  Serial.println("Static IP configured");

/*   // WiFi setup - connect to existing
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(DELAYVALUE/1);
    Serial.print(".");
  } */
  // WiFi setup - create own
  WiFi.beginAP(ssid, password);
  while (WiFi.status() != WL_AP_LISTENING) {
    delay(DELAYVALUE/1);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.println("IP address: " + WiFi.localIP().toString());

  // Start UDP listening server - receive sensor reading
  if (udp.begin(localPort)) { // start listening on udpPort
    Serial.print("UDP listening on port ");
    Serial.println(localPort);
  } else {
    Serial.println("Failed to start");
  }
}
void loop() {

  int packetSize = udp.parsePacket();
  if (packetSize >= 2) { // expect sensorID (1 bytes) + brightness (1 byte)
	sensorID = udp.read();
	brightness = udp.read();     
    // Simple mapping: sensorID directly to LED indices
    for (uint8_t i = 0; i < LEDS_PER_ZONE; i++) {
        ledIndex = ledIdx[sensorID-1][i];
        if (ledIndex < LED_COUNT) {
            pixels[ledIndex] = CRGB(brightness, brightness, brightness); // Set LED brightness (white color)
            Serial.println("Sensor " + String(sensorID) + " -> LED " + String(ledIndex) + " brightness: " + String(brightness));
        }
    }
  }
  FastLED.show();
  delay(DELAYVALUE/10);  // poll every _
}