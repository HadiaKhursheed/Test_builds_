Wave illusion:
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

float offset = 0;

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
}

void loop() {
  display.clearDisplay();

  for (int x = 0; x < SCREEN_WIDTH; x += 4) {
    int y = 32 + sin((x + offset) * 0.1) * 20;
    display.drawPixel(x, y, SSD1306_WHITE);
  }

  display.display();
  offset += 2;
  delay(30);
}

