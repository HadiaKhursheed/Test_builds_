#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Arduino_JSON.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TM1637Display.h>
#include <time.h> // Required for time_t and tm structure to get date components

// --- Display and Primary Control Buttons ---
#define CLK D1          // TM1637 CLK pin (GPIO5)
#define DIO D2          // TM1637 DIO pin (GPIO4)
#define BUTTON_1_PIN D3   // Cycles modes (Time/Temp/Alarm Set)
#define BUTTON_2_PIN D4   // Increments (in set mode) / Stops alarm
#define BUTTON_3_PIN D5   // Adjusts display brightness
#define BUTTON_4_PIN D6   // Displays current date
#define BUZZER_PIN D7   // Pin for the buzzer

// --- Wi-Fi and API Credentials ---
const char* ssid = "Airtel_Prime";      // !! IMPORTANT: Replace with your Wi-Fi SSID !!
const char* password = "PohawithCake900";    // !! IMPORTANT: Replace with your Wi-Fi Password !!
const char* apiKey = "e911d0203a0bcab42db9400116065775"; // !! IMPORTANT: Replace with your OpenWeatherMap API Key !!
const char* city = "Aligarh";            // City for weather data
const char* countryCode = "IN";          // Country Code for weather data

// --- Global Objects ---
TM1637Display display(CLK, DIO);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800); // 19800 seconds = 5 hours 30 minutes for IST

// --- Global Variables ---
int temperature = 0;
int displayMode = 0; // 0: Time, 1: Temperature, 2: Alarm Set Mode
unsigned long lastTemperatureUpdate = 0; // Tracks last temperature fetch time
uint8_t currentDisplayBrightness = 7; // Stores current brightness level

// --- Alarm Variables ---
int alarmHour = 7;
int alarmMinute = 0;
bool alarmEnabled = true;
bool alarmSetMode = false;
int alarmSetStage = 0;
bool alarmTriggered = false;
unsigned long alarmTriggerTime = 0;
const unsigned long ALARM_DURATION = 30000; // Alarm sounds for 30 seconds

// --- Debounce Variables ---
unsigned long lastButton1Press = 0;
unsigned long lastButton2Press = 0;
unsigned long lastButton3Press = 0;
unsigned long lastButton4Press = 0;
const unsigned long DEBOUNCE_DELAY = 200; // Milliseconds for button debouncing

// --- Animation Constants ---
const uint8_t SEG_UPPER_HALF_8 = 0x63; // Segments a, b, f, g for animation
const uint8_t SEG_LOWER_HALF_8 = 0x5C; // Segments c, d, e, g for animation
const uint8_t SEG_ALL_OFF = 0x00;      // All segments off

const uint8_t ANIMATION_FRAMES[][4] = {
  {SEG_ALL_OFF, SEG_ALL_OFF, SEG_ALL_OFF, SEG_ALL_OFF},
  {SEG_UPPER_HALF_8, SEG_ALL_OFF, SEG_ALL_OFF, SEG_ALL_OFF},
  {SEG_UPPER_HALF_8, SEG_LOWER_HALF_8, SEG_ALL_OFF, SEG_ALL_OFF},
  {SEG_UPPER_HALF_8, SEG_LOWER_HALF_8, SEG_UPPER_HALF_8, SEG_ALL_OFF},
  {SEG_UPPER_HALF_8, SEG_LOWER_HALF_8, SEG_UPPER_HALF_8, SEG_LOWER_HALF_8},
  {SEG_ALL_OFF, SEG_ALL_OFF, SEG_ALL_OFF, SEG_UPPER_HALF_8},
  {SEG_ALL_OFF, SEG_ALL_OFF, SEG_LOWER_HALF_8, SEG_UPPER_HALF_8},
  {SEG_ALL_OFF, SEG_UPPER_HALF_8, SEG_LOWER_HALF_8, SEG_UPPER_HALF_8},
  {SEG_LOWER_HALF_8, SEG_UPPER_HALF_8, SEG_LOWER_HALF_8, SEG_UPPER_HALF_8},
  {SEG_ALL_OFF, SEG_ALL_OFF, SEG_ALL_OFF, SEG_ALL_OFF},
  {SEG_ALL_OFF, SEG_ALL_OFF, SEG_ALL_OFF, SEG_ALL_OFF}
};

const int NUM_ANIMATION_FRAMES = sizeof(ANIMATION_FRAMES) / sizeof(ANIMATION_FRAMES[0]);
const int ANIMATION_DELAY_MS = 150;

// --- Variables for Button 4 (Date Display) ---
bool showDateActive = false; // Flag to indicate if date display is active
unsigned long showDateStartTime = 0; // Time when date display started
const unsigned long DATE_DISPLAY_DURATION = 2000; // Display date for 2 seconds (2000 ms)

// --- Function Declarations ---
void fetchTemperature();
void displayTime();
void displayTemperature();
void displayAlarmTimeForSetting();
void buzzAlarm();
void stopAlarm();

// ===================================================================================================
// SETUP FUNCTION
// ===================================================================================================
void setup() {
  // --- Initial Wi-Fi Connection Start (non-blocking) ---
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // --- NTP Client Initialization (begin early) ---
  timeClient.begin();

  // --- Animation Sequence on Startup ---
  display.setBrightness(0x0c); // Set brightness specifically for the animation
  display.clear(); // Clear the display before animation

  for (int i = 0; i < NUM_ANIMATION_FRAMES; i++) {
    display.setSegments(ANIMATION_FRAMES[i], 4, 0);
    // The delay() function allows the ESP8266's Wi-Fi tasks to run in the background
    delay(ANIMATION_DELAY_MS);

    // During animation, if Wi-Fi connects, try to update time
    if (WiFi.status() == WL_CONNECTED) {
      if (!timeClient.isTimeSet()) { // Only update if time isn't set yet
         timeClient.update();
      }
    }
  }

  // --- After Animation: Final Wi-Fi/Time Check & Temperature Fetch ---
  currentDisplayBrightness = 7; // Initialize the brightness tracking variable
  display.setBrightness(currentDisplayBrightness); // Set initial brightness for regular clock operation
  display.clear(); // Clear display after animation, ready for clock face

  if (WiFi.status() != WL_CONNECTED) {
    unsigned long wifiConnectStart = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - wifiConnectStart < 15000) { // Wait up to 15 seconds
      delay(500); // Small delay to allow Wi-Fi to connect
    }
  }

  if (WiFi.status() == WL_CONNECTED && !timeClient.isTimeSet()) {
    timeClient.update(); // Force update
  }

  // Fetch initial temperature after setup (if Wi-Fi is connected)
  if (WiFi.status() == WL_CONNECTED) {
    fetchTemperature();
    lastTemperatureUpdate = millis(); // Set initial update time
  }

  // --- Pin Modes ---
  pinMode(BUTTON_1_PIN, INPUT_PULLUP); // Button 1 connected between pin and GND
  pinMode(BUTTON_2_PIN, INPUT_PULLUP); // Button 2 connected between pin and GND
  pinMode(BUTTON_3_PIN, INPUT_PULLUP); // Button 3 connected between pin and GND
  pinMode(BUTTON_4_PIN, INPUT_PULLUP); // Button 4 connected between pin and GND
  pinMode(BUZZER_PIN, OUTPUT);         // Buzzer pin as output
  digitalWrite(BUZZER_PIN, LOW);       // Ensure buzzer is off initially
}

// ===================================================================================================
// UTILITY FUNCTIONS
// ===================================================================================================

// Fetches current temperature from OpenWeatherMap API
void fetchTemperature() {
  if (WiFi.status() == WL_CONNECTED) { // Only fetch if Wi-Fi is connected
    WiFiClient client;
    HTTPClient http;
    // Construct the API URL to get temperature in Celsius (units=metric)
    String url = "http://api.openweathermap.org/data/2.5/weather?q=" + String(city) + "," + String(countryCode) + "&APPID=" + String(apiKey) + "&units=metric";

    http.begin(client, url.c_str()); // Begin HTTP connection
    int httpCode = http.GET(); // Send GET request

    if (httpCode == HTTP_CODE_OK) { // Check if request was successful (HTTP 200)
      String payload = http.getString(); // Get the response payload (JSON string)
      JSONVar jsonData = JSON.parse(payload); // Parse JSON response

      // Check if JSON parsing was successful and required data exists
      if (JSON.typeof(jsonData) != "undefined" && jsonData.hasOwnProperty("main") && jsonData["main"].hasOwnProperty("temp")) {
        temperature = (int)jsonData["main"]["temp"]; // Extract temperature and cast to int
      }
    }
    http.end(); // Close HTTP connection
  }
}

// Displays current time on the TM1637 display
void displayTime() {
  timeClient.update(); // Update time from NTP server
  int hours = timeClient.getHours();
  int minutes = timeClient.getMinutes();
  int seconds = timeClient.getSeconds();
  int displayValue = hours * 100 + minutes; // Combine hours and minutes for display

  // Blinking colon effect based on seconds
  if (seconds % 2 == 0) {
    display.showNumberDecEx(displayValue, 0b01000000, false, 4, 0); // Show colon
  } else {
    display.showNumberDecEx(displayValue, 0b00000000, false, 4, 0); // Hide colon
  }
}

// Displays temperature with a Celsius symbol on the TM1637 display
void displayTemperature() {
  display.showNumberDec(temperature, false, 2, 0); // Display temperature (e.g., "25") in first two digits
  // Custom segments for Celsius symbol (°C)
  // 0x63 = degree symbol (seg G, F, B, A)
  // 0x39 = 'C' (seg F, E, D, C, B)
  uint8_t celsius[] = {0x63, 0x39};
  display.setSegments(celsius, 2, 2); // Display Celsius symbol starting from the 3rd digit (index 2)
}

// Displays alarm time for setting, with flashing digits for the current setting stage
void displayAlarmTimeForSetting() {
  int displayAlarmValue = alarmHour * 100 + alarmMinute;

  if (alarmSetStage == 0) { // Stage 0: Setting Hour - flash hour digits
    if ((millis() / 250) % 2 == 0) { // Flash every 250ms
      display.showNumberDecEx(displayAlarmValue, 0b01000000, false, 4, 0); // Show full time with colon
    } else {
      // Hide hour digits to make them flash, keep colon and minute
      display.showNumberDecEx(alarmMinute, 0b01000000, false, 2, 2); // Display only minutes, starting at pos 2
      uint8_t blank[] = {0,0}; // Blank first two digits for hours
      display.setSegments(blank, 2, 0);
    }
  } else if (alarmSetStage == 1) { // Stage 1: Setting Minute - flash minute digits
    if ((millis() / 250) % 2 == 0) { // Flash every 250ms
      display.showNumberDecEx(displayAlarmValue, 0b01000000, false, 4, 0); // Show full time with colon
    } else {
      // Hide minute digits to make them flash, keep colon and hour
      display.showNumberDecEx(alarmHour, 0b01000000, false, 2, 0); // Display only hours, starting at pos 0
      uint8_t blank[] = {0,0}; // Blank last two digits for minutes
      display.setSegments(blank, 2, 2);
    }
  }
}

// Manages the buzzing behavior when the alarm is triggered
void buzzAlarm() {
  if (!alarmTriggered) { // If alarm just started ringing
    alarmTriggered = true;
    alarmTriggerTime = millis(); // Record the start time of the alarm
  }

  // Toggle buzzer on/off to create a beeping sound
  if ((millis() / 200) % 2 == 0) { // Beep every 200ms
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  // Stop alarm after ALARM_DURATION
  if (millis() - alarmTriggerTime >= ALARM_DURATION) {
    stopAlarm(); // Turn off buzzer and reset state
  }
}

// Stops the alarm (turns off buzzer and resets state)
void stopAlarm() {
  digitalWrite(BUZZER_PIN, LOW); // Turn off buzzer
  alarmTriggered = false;         // Reset alarm trigger state
}

// ===================================================================================================
// MAIN LOOP FUNCTION - Runs repeatedly
// ===================================================================================================
void loop() {
  // --- Temperature Update Logic ---
  // Only try to fetch temperature if WiFi is connected and enough time has passed
  if (WiFi.status() == WL_CONNECTED && (millis() - lastTemperatureUpdate >= 900000 || lastTemperatureUpdate == 0)) {
    fetchTemperature();
    lastTemperatureUpdate = millis(); // Update timestamp after attempt to fetch
  }

  // --- Alarm Check ---
  timeClient.update(); // Keep time updated from NTP server (will work once WiFi connects)
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();

  // Trigger alarm if enabled, not already triggered, not in set mode, and time matches at 0 seconds
  if (alarmEnabled && !alarmTriggered && !alarmSetMode &&
      currentHour == alarmHour && currentMinute == alarmMinute && currentSecond == 0) {
    buzzAlarm(); // Start buzzing
  }

  // If alarm is currently triggered, continue buzzing logic
  if (alarmTriggered) {
    buzzAlarm();
  }

  // --- Button Handling ---
  unsigned long currentTime = millis(); // Get current time for debouncing

  // Button 1 (D3) Logic: Cycles modes / Advances alarm set stage
  if (digitalRead(BUTTON_1_PIN) == LOW && currentTime - lastButton1Press > DEBOUNCE_DELAY) {
    lastButton1Press = currentTime; // Update last press time for debouncing
    stopAlarm(); // Stop alarm if it's ringing when a mode change button is pressed

    if (!alarmSetMode) { // If not currently in alarm setting mode
      displayMode = (displayMode + 1) % 3; // Cycle: 0 (Time) -> 1 (Temp) -> 2 (Alarm Set)
      if (displayMode == 2) { // If just entered Alarm Set Mode
        alarmSetMode = true; // Set flag for alarm setting
        alarmSetStage = 0; // Start at setting hour stage
      }
    } else { // Already in alarm set mode, Button 1 advances the setting stage
        alarmSetStage++;
        if (alarmSetStage > 1) { // After setting minutes, exit set mode
            alarmSetMode = false; // Exit alarm setting mode
            displayMode = 0; // Go back to time display
        }
    }
    display.clear(); // Clear display to prepare for new mode
  }

  // --- Button 2 (D4) Logic: Stop Alarm OR Increment Values ---
  if (digitalRead(BUTTON_2_PIN) == LOW && currentTime - lastButton2Press > DEBOUNCE_DELAY) {
    lastButton2Press = currentTime; // Update last press time for debouncing

    if (alarmTriggered) { // PRIORITY 1: If alarm is currently ringing, stop it
      stopAlarm();
    } else if (alarmSetMode) { // PRIORITY 2: If in alarm setting mode, increment values
      if (alarmSetStage == 0) { // Currently setting Hour
        alarmHour = (alarmHour + 1) % 24; // Increment hour (0-23 cycle)
      } else if (alarmSetStage == 1) { // Currently setting Minute
        alarmMinute = (alarmMinute + 1) % 60; // Increment minute (0-59 cycle)
      }
    }
    // If not alarm triggered and not in alarm set mode, Button 2 does nothing
  }

  // --- Button 3 (D5) Logic: Adjust Display Brightness ---
  if (digitalRead(BUTTON_3_PIN) == LOW && currentTime - lastButton3Press > DEBOUNCE_DELAY) {
    lastButton3Press = currentTime; // Update last press time for debouncing

    // Use the global variable to cycle and set brightness
    currentDisplayBrightness = (currentDisplayBrightness + 1) % 8; // Increment and loop from 0 to 7
    display.setBrightness(currentDisplayBrightness); // Set the new brightness

    // Briefly show current brightness level on display
    display.showNumberDec(currentDisplayBrightness, false, 1, 0); // Shows a single digit (0-7) at the beginning of the display
    delay(500); // Show brightness level for half a second
    display.clear(); // Clear the brightness number before the main display logic takes over
  }

  // --- Button 4 (D6) Logic: Display Current Date (DD/MM) ---
  if (digitalRead(BUTTON_4_PIN) == LOW && currentTime - lastButton4Press > DEBOUNCE_DELAY) {
    lastButton4Press = currentTime; // Update last press time for debouncing

    showDateActive = true; // Activate date display
    showDateStartTime = currentTime; // Record the start time

    timeClient.update(); // Make sure time is updated for accurate date
    time_t epochTime = timeClient.getEpochTime(); // Get Unix epoch time

    // Convert epoch time to a tm structure for date extraction (local time)
    struct tm *ptm = localtime(&epochTime);

    int day = ptm->tm_mday;   // Day of the month (1-31)
    int month = ptm->tm_mon + 1; // Month (0-11, so add 1 for 1-12)

    // Format for DD/MM display: e.g., 01/06 or 25/12
    // Combine day and month into a 4-digit number (DDMM) and use colon
    display.showNumberDecEx((day * 100) + month, 0b01000000, false, 4, 0); // Show DD/MM with colon
  }

  // Check if Button 4's date display should expire
  if (showDateActive && (currentTime - showDateStartTime >= DATE_DISPLAY_DURATION)) {
    showDateActive = false; // Deactivate the date display
    display.clear(); // Clear display after date display expires
  }

  // --- Display Logic ---
  // This ensures the date display takes precedence when active
  if (showDateActive) {
    // Do nothing here, the display is handled within the button press
    // and then cleared by the expiration check above.
  } else if (alarmTriggered) {
    displayTime(); // If alarm is sounding, show current time
  } else if (alarmSetMode) {
    displayAlarmTimeForSetting(); // If in alarm set mode, show alarm time
  } else {
    // Otherwise, show time or temperature based on displayMode
    if (displayMode == 0) {
      displayTime();
    } else { // displayMode == 1
      displayTemperature();
    }
  }

  delay(50); // Small delay to prevent rapid loop execution and stabilize readings. This also allows Wi-Fi background tasks.
}
