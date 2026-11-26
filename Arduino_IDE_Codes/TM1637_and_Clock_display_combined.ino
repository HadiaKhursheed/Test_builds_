// TM1637 display + strip light digit display combined code
#include <Wire.h>
#include <RTClib.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>
#include <TM1637Display.h>
#define LED_PIN D4
#define NUM_LEDS 130  
#define DIGITS 6
#define SEGMENTS 7
#define SEGMENT_LEDS 3
#define FIRST_COLON_INDEX 42
#define SECOND_COLON_INDEX 86
#define MINUTE_LEDS_START 44
#define SECOND_LEDS_START 88
#define TM1637_CLK D5   // Changed TM1637 pins to D5 (CLK) and D6 (DIO) to avoid I2C conflict
#define TM1637_DIO D6
const char* ssid = "Airtel_airtel";
const char* password = "Connected37";
const long utcOffsetInSeconds = 19800;  // IST +5:30
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
RTC_DS1307 rtc;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
TM1637Display display(TM1637_CLK, TM1637_DIO);
uint8_t segmentMap[10][SEGMENTS] = {
  {1, 1, 1, 0, 1, 1, 1}, // 0
  {0, 0, 1, 0, 0, 0, 1}, // 1
  {1, 1, 0, 1, 0, 1, 1}, // 2
  {0, 1, 1, 1, 0, 1, 1}, // 3
  {0, 0, 1, 1, 1, 0, 1}, // 4
  {0, 1, 1, 1, 1, 1, 0}, // 5
  {1, 1, 1, 1, 1, 1, 0}, // 6
  {0, 0, 1, 0, 0, 1, 1}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {0, 1, 1, 1, 1, 1, 1}, // 9
};
unsigned long lastNtpSync = 0;
const unsigned long ntpSyncInterval = 3600000; // sync every hour
void setup() {
  Serial.begin(115200);
  Wire.begin(D2, D1); // I2C pins for RTC
  strip.begin();
  strip.show();
  display.setBrightness(7);
  display.clear();
  Serial.println("Initializing RTC...");
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running, will sync with NTP...");
  } else {
    Serial.println("RTC found and running.");
  }
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected!");
  timeClient.begin();
  // Sync RTC for first time if not running or if startup before first hour to keep accurate
  syncRtcWithNtp();
  lastNtpSync = millis();
}
void loop() {
  // Periodically sync RTC with NTP (every hour)
  if (millis() - lastNtpSync > ntpSyncInterval) {
    Serial.println("Syncing RTC with NTP...");
    syncRtcWithNtp();
    lastNtpSync = millis();
  }
  DateTime now = rtc.now();
  int hour = now.hour();
  int minute = now.minute();
  int second = now.second();
  Serial.printf("Time: %02d:%02d:%02d\n", hour, minute, second);
  displayTimeOnStrip(hour, minute, second);
  displayTimeOnTM1637(hour, minute, second);
  delay(1000);
}
void syncRtcWithNtp() {
  if (timeClient.forceUpdate()) {
    DateTime ntpTime = DateTime(timeClient.getEpochTime());
    rtc.adjust(ntpTime);
    Serial.printf("RTC set to %04d-%02d-%02d %02d:%02d:%02d\n",
                  ntpTime.year(), ntpTime.month(), ntpTime.day(),
                  ntpTime.hour(), ntpTime.minute(), ntpTime.second());
  } else {
    Serial.println("NTP sync failed.");
  }
}
void displayTimeOnStrip(int hour, int minute, int second) {
  strip.clear();
  int digits[DIGITS] = {hour / 10, hour % 10, minute / 10, minute % 10, second / 10, second % 10};
  uint32_t digitColor = strip.Color(255, 0, 0);
  uint32_t colonColor = strip.Color(0, 0, 40);
  for (int i = 0; i < 2; i++) {
    displayDigit(digits[i], i * SEGMENTS * SEGMENT_LEDS, digitColor);
  }
  strip.setPixelColor(FIRST_COLON_INDEX, colonColor);
  strip.setPixelColor(FIRST_COLON_INDEX + 1, colonColor);
  for (int i = 2; i < 4; i++) {
    displayDigit(digits[i], MINUTE_LEDS_START + (i - 2) * SEGMENTS * SEGMENT_LEDS, digitColor);
  }
  strip.setPixelColor(SECOND_COLON_INDEX, colonColor);
  strip.setPixelColor(SECOND_COLON_INDEX + 1, colonColor);
  for (int i = 4; i < 6; i++) {
    displayDigit(digits[i], SECOND_LEDS_START + (i - 4) * SEGMENTS * SEGMENT_LEDS, digitColor);
  }
  strip.show();
}
void displayDigit(int number, int startIndex, uint32_t color) {
  for (int seg = 0; seg < SEGMENTS; seg++) {
    if (segmentMap[number][seg]) {
      for (int led = 0; led < SEGMENT_LEDS; led++) {
        strip.setPixelColor(startIndex + seg * SEGMENT_LEDS + led, color);
      }
    }
  }
}
void displayTimeOnTM1637(int hour, int minute, int second) {
  int timeValue = hour * 100 + minute;
  uint8_t colonFlag = (second % 2 == 0) ? 0b01000000 : 0b00000000;
  display.showNumberDecEx(timeValue, colonFlag, false, 4, 0);
}



 
