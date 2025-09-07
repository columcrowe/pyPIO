//#include <Adafruit_NeoPixel.h>
#include <FastLED.h>
#define PIN 3
#define N_STRIPS 1
#define N_LEDS 60
#define LED_COUNT N_STRIPS*N_LEDS
#define BATCH_SIZE 12 // set
#define DELAYVAL 1000

CRGB pixels[LED_COUNT]; // using FastLED lib > NeoPixel, lower-level control
int currentLED = 0;     // for tracking

void setup() {
  FastLED.addLeds<WS2812, PIN, GRB>(pixels, LED_COUNT);
  delay(DELAYVAL);  // Wait for power stabilization
  FastLED.setBrightness(255);

  // Clear all LEDs initially
  FastLED.clear();
  FastLED.show();
}
void glow(int ledIndex, int tempo) {
  // Clear all LEDs first
  FastLED.clear();

  // Fade brightness up for the specific LED
  for (int i = 0; i < 255; i++) {
    pixels[ledIndex] = CRGB(i, i, i);  // Directly control LED brightness
    FastLED.show();
    delay(tempo / 255);  // Divide tempo to make fade smooth
  }

  // Fade brightness down for the specific LED
  for (int i = 255; i >= 0; i--) {
    pixels[ledIndex] = CRGB(i, i, i);
    FastLED.show();
    delay(tempo / 255);
  }
}
void glowBatch(int startIndex, int batchSize, int tempo) {
  // Clear all LEDs first
  FastLED.clear();

  for (int brightness = 0; brightness < 255; brightness++) {
    for (int i = 0; i < batchSize; i++) {
      int ledIndex = startIndex + i;
      if (ledIndex >= LED_COUNT) {
        ledIndex = ledIndex - LED_COUNT;
      }
      pixels[ledIndex] = CRGB(brightness, brightness, brightness);
    }
    FastLED.show();
    delay(tempo / 255);
  }

  for (int brightness = 255; brightness >= 0; brightness--) {
    for (int i = 0; i < batchSize; i++) {
      int ledIndex = startIndex + i;
      if (ledIndex >= LED_COUNT) {
        ledIndex = ledIndex - LED_COUNT;
      }
      pixels[ledIndex] = CRGB(brightness, brightness, brightness);
    }
    FastLED.show();
    delay(tempo / 255);
  }
}
void loop() {

  //glow(currentLED, DELAYVAL);
  glowBatch(currentLED, BATCH_SIZE, DELAYVAL);
  //currentLED++;   // Move to next LED
  currentLED += BATCH_SIZE; // Move to next batch (jump by full batch size)

  // Reset to beginning when we reach the end
  if (currentLED >= LED_COUNT) {
    currentLED = 0;
  }
}