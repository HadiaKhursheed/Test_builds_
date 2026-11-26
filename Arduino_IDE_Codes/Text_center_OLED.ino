// Text in center
#include <Wire.h>
#include <SSD1306Wire.h>  // Install "ESP8266 and ESP32 OLED driver for SSD1306 displays" by ThingPulse

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

SSD1306Wire display(0x3C, D2, D1);  // SDA = D2, SCL = D1

void setup() {
  display.init();
  display.flipScreenVertically();  // Optional
  display.setContrast(255);

  display.clear();
  display.setFont(ArialMT_Plain_16);  // Choose font size
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(SCREEN_WIDTH / 2, 20, "Hello, Ahmad!");
  display.display();
}

void loop() {
  // Nothing here for now
}

