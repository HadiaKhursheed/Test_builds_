#include <FastLED.h>

#define NUM_LEDS 130
#define LED_PIN  D4

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
  fill_solid(leds, NUM_LEDS, CRGB::Red);
  FastLED.show();
}

void loop() {
  // Static color is already displayed; add animations or updates here if needed.
}
