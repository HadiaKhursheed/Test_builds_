# Arduino IDE Codes

Tested, working `.ino` sketches saved for quick reuse.

| # | Sketch | Hardware / Board | What It Does |
| :--- | :--- | :--- | :--- |
| 1 | [`strip_light_board_display.ino`](./strip_light_board_display.ino) | ESP8266 + WS2812B | Prototype 11×10 LED strip display board (11×5 active) that displays text entered via a phone connected to its Wi-Fi. |
| 2 | [`TM1637_and_Clock_display_combined.ino`](./TM1637%26Clock_display_combined.ino) | ESP8266 + TM1637 + Strip | Combines a TM1637 7-segment display and an LED strip digit display with RTC and NTP time sync. |
| 3 | [`TM1637_starting_animation.ino`](./TM1637_starting_animation.ino) | ESP8266 + TM1637 | Plays a startup/loading animation on a TM1637 clock display. |
| 4 | [`Turn_OnOff_LED_onWeb.ino`](./Turn_OnOff_LED_onWeb.ino) | ESP8266 + LED | Toggles an LED on and off over Wi-Fi from a local webpage with no physical switch. |
| 5 | [`Automatic_stickman_jump.ino`](./Automatic_stickman_jump.ino) | ESP8266 + OLED | Tests an automated stickman jump animation on an OLED screen. |
| 6 | [`Text_on_OLED.ino`](./Text_on_OLED.ino) | ESP8266 + OLED | Displays basic text on an OLED screen. |
| 7 | [`Lightup_full_OLED.ino`](./Lightup_full_OLED.ino) | ESP8266 + OLED | Lights up the full OLED screen to check for dead pixels. |
| 8 | [`Spin_illusion_OLED.ino`](./Spin_illusion_OLED.ino) | ESP8266 + OLED | Uses math functions to render a spinning visual illusion on OLED. |
| 9 | [`Wave_illusion_OLED.ino`](./Wave_illusion_OLED.ino) | ESP8266 + OLED | Uses math functions to render a wave visual illusion on OLED. |
| 10 | [`Text_center_OLED.ino`](./Text_center_OLED.ino) | ESP8266 + OLED | Automatically centers any text string on an OLED display. |
| 11 | [`blinking_LED.ino`](./blinking_LED.ino) | Arduino / ESP | Basic LED blink test sketch. |
| 12 | [`flashing_LED.ino`](./flashing_LED.ino) | Arduino / ESP | LED flashing pattern test sketch. |
