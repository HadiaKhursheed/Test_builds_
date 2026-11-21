// TM1637 display starting animation
// Include the TM1637 display library
// You can install this library via Arduino IDE: Sketch -> Include Library -> Manage Libraries...
// Search for "TM1637Display" (e.g., by Avishay Nehemiah)
#include <TM1637Display.h>
// Define the CLK and DIO pins for the TM1637 display
// For ESP8266 NodeMCU/Wemos D1 Mini:
// D1 corresponds to GPIO5
// D2 corresponds to GPIO4
#define CLK 8  // GPIO5
#define DIO 9  // GPIO4
// Create a TM1637Display object with the defined pins
TM1637Display display(CLK, DIO);
// Define segment patterns for the upper and lower halves of an '8'
// Segment bit mapping (0bDPGEDCBA):
//   --a--
//  |     |
//  f     b
//  |     |
//   --g--
//  |     |
//  e     c
//  |     |
//   --d--  dp (decimal point)
//
// Full '8' = 0x7F (all segments a,b,c,d,e,f,g)
//
// Upper half of '8' (segments a, b, f, g) = 0x63
//   a (0x01) | b (0x02) | f (0x20) | g (0x40) = 0x63
//
// Lower half of '8' (segments c, d, e, g) = 0x5C
//   c (0x04) | d (0x08) | e (0x10) | g (0x40) = 0x5C
const uint8_t SEG_UPPER_HALF_8 = 0x63; // Segments a, b, f, g
const uint8_t SEG_LOWER_HALF_8 = 0x5C; // Segments c, d, e, g
const uint8_t SEG_ALL_OFF = 0x00;      // All segments off
const uint8_t ANIMATION_FRAMES[][4] = {
  // --- Forward Glowing Sequence (Left to Right, Alternating Half-8) ---
  {SEG_ALL_OFF, SEG_ALL_OFF, SEG_ALL_OFF, SEG_ALL_OFF}, // Frame 0: All off (start clean)
  {SEG_UPPER_HALF_8, SEG_ALL_OFF, SEG_ALL_OFF, SEG_ALL_OFF}, // Frame 1: Digit 1 - Upper half glows
  {SEG_UPPER_HALF_8, SEG_LOWER_HALF_8, SEG_ALL_OFF, SEG_ALL_OFF}, // Frame 2: Digit 1 - Upper, Digit 2 - Lower glows
  {SEG_UPPER_HALF_8, SEG_LOWER_HALF_8, SEG_UPPER_HALF_8, SEG_ALL_OFF}, // Frame 3: Digit 1 - Upper, Digit 2 - Lower, Digit 3 - Upper glows
  {SEG_UPPER_HALF_8, SEG_LOWER_HALF_8, SEG_UPPER_HALF_8, SEG_LOWER_HALF_8}, // Frame 4: Digit 1 - Upper, Digit 2 - Lower, Digit 3 - Upper, Digit 4 - Lower glows (All in pattern)
  // --- Backward Glowing Sequence (Right to Left, Alternating Half-8) ---
  {SEG_ALL_OFF, SEG_ALL_OFF, SEG_ALL_OFF, SEG_UPPER_HALF_8}, // Frame 7: Digit 4 - Upper half glows
  {SEG_ALL_OFF, SEG_ALL_OFF, SEG_LOWER_HALF_8, SEG_UPPER_HALF_8}, // Frame 8: Digit 3 - Lower, Digit 4 - Upper glows
  {SEG_ALL_OFF, SEG_UPPER_HALF_8, SEG_LOWER_HALF_8, SEG_UPPER_HALF_8}, // Frame 9: Digit 2 - Upper, Digit 3 - Lower, Digit 4 - Upper glows
  {SEG_LOWER_HALF_8, SEG_UPPER_HALF_8, SEG_LOWER_HALF_8, SEG_UPPER_HALF_8}, // Frame 10: Digit 1 - Lower, Digit 2 - Upper, Digit 3 - Lower, Digit 4 - Upper glows (All in pattern)
  // --- Pause before repeating the cycle ---
  {SEG_ALL_OFF, SEG_ALL_OFF, SEG_ALL_OFF, SEG_ALL_OFF}, // Frame 11: All off
  {SEG_ALL_OFF, SEG_ALL_OFF, SEG_ALL_OFF, SEG_ALL_OFF}  // Frame 12: Extended pause
};
// Number of animation frames
const int NUM_ANIMATION_FRAMES = sizeof(ANIMATION_FRAMES) / sizeof(ANIMATION_FRAMES[0]);
// Delay between each animation frame in milliseconds
const int ANIMATION_DELAY_MS = 150; // Adjusted for a clear sequential glow
void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);
  Serial.println("ESP8266 TM1637 Custom Half-8 Alternating Animation");
  // Set the brightness of the display (0-7, 7 is brightest)
  // 0x0c corresponds to 4/8 brightness (from 0x00 to 0x0f in the library)
  display.setBrightness(0x0c);
  // Clear the display initially
  display.clear();
  Serial.println("Display initialized and cleared.");
}
void loop() {
  // Iterate through each animation frame
  for (int i = 0; i < NUM_ANIMATION_FRAMES; i++) {
    // Set the segments for all 4 digits using the current animation frame
    // The arguments are: segments array, number of segments (4 for a 4-digit display),
    // and the starting position (0 for the first digit)
    display.setSegments(ANIMATION_FRAMES[i], 4, 0);
    // Print the current frame to Serial Monitor for debugging
    Serial.print("Displaying frame: ");
    for (int j = 0; j < 4; j++) {
      Serial.print(ANIMATION_FRAMES[i][j], HEX);
      Serial.print(" ");
    }
    Serial.println();
    // Wait for a short period before showing the next frame
    delay(ANIMATION_DELAY_MS);
  }
}

