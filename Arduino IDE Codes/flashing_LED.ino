const int ledPins[] = {D1, D2, D3, D4};
// Add more pin numbers as needed
 void setup() {
    // Initialize all LED pins as output
    for (int pin : ledPins) {
        pinMode(pin, OUTPUT);
    }
}
 void loop() {
    // Turn on each LED, delay, then turn it off
    for (int pin : ledPins) {
        digitalWrite(pin, HIGH);
        delay(300);
        digitalWrite(pin, LOW);
    }
}
