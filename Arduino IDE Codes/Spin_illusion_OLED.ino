// Spinning illusion
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

float angle = 0;

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
}

void loop() {
  display.clearDisplay();

  int centerX = SCREEN_WIDTH / 2;
  int centerY = SCREEN_HEIGHT / 2;

  for (int i = 0; i < 64; i += 4) {
    float x1 = centerX + cos(angle + i * 0.1) * i;
    float y1 = centerY + sin(angle + i * 0.1) * i;
    float x2 = centerX + cos(angle + i * 0.1 + PI) * i;
    float y2 = centerY + sin(angle + i * 0.1 + PI) * i;

    display.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
  }

  display.display();
  angle += 0.1;
  delay(30);
}


Rotating spin illusion:
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

float angle = 0;

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
}

void loop() {
  display.clearDisplay();
  int cx = SCREEN_WIDTH / 2;
  int cy = SCREEN_HEIGHT / 2;

  for (int r = 0; r < 64; r += 4) {
    float x = cos(angle + r * 0.1) * r;
    float y = sin(angle + r * 0.1) * r;
    display.drawPixel(cx + x, cy + y, SSD1306_WHITE);
  }

  display.display();
  angle += 0.1;
  delay(30);
}
