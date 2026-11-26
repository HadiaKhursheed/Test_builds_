#include <FastLED.h>

#define NUM_LEDS 130
#define LED_PIN  D4

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
}

void loop() {
  leds[0]= CRGB::Red;
  leds[1]= CRGB::Red;
  leds[2]= CRGB::Red;
  leds[3]= CRGB::Red;
  leds[4]= CRGB::Red;
  leds[5]= CRGB::Red;
  leds[6]= CRGB::Red;
  leds[7]= CRGB::Red;
  leds[8]= CRGB::Red;
  leds[9]= CRGB::Red;
  leds[10]= CRGB::Red;
  leds[11]= CRGB::Red;
  leds[12]= CRGB::Red;
  leds[13]= CRGB::Red;
  leds[14]= CRGB::Red;
  leds[15]= CRGB::Red;
  leds[16]= CRGB::Red;
  leds[17]= CRGB::Red;
  leds[18]= CRGB::Red;
  leds[19]= CRGB::Red;
  leds[20]= CRGB::Red;
  leds[21]= CRGB::Red;
  leds[22]= CRGB::Red;
  leds[23]= CRGB::Red;
  leds[24]= CRGB::Red;
  leds[25]= CRGB::Red;
  leds[26]= CRGB::Red;
  leds[27]= CRGB::Red;
  leds[28]= CRGB::Red;
  leds[29]= CRGB::Red;
  leds[30]= CRGB::Red;
  leds[31]= CRGB::Red;
  leds[32]= CRGB::Red;
  leds[33]= CRGB::Red;
  leds[34]= CRGB::Red;
  leds[35]= CRGB::Red;
  leds[36]= CRGB::Red;
  leds[37]= CRGB::Red;
  leds[38]= CRGB::Red;
  leds[39]= CRGB::Red;
  leds[40]= CRGB::Red;
  leds[41]= CRGB::Red;
  leds[42]= CRGB::Red;
  leds[43]= CRGB::Red;
  leds[44]= CRGB::Red;
  leds[45]= CRGB::Red;
  leds[46]= CRGB::Red;
  leds[47]= CRGB::Red;
  leds[48]= CRGB::Red;
  leds[49]= CRGB::Red;
  leds[50]= CRGB::Red;
  leds[51]= CRGB::Red;
  leds[52]= CRGB::Red;
  leds[53]= CRGB::Red;
  leds[54]= CRGB::Red;
  leds[55]= CRGB::Red;
  leds[56]= CRGB::Red;
  leds[57]= CRGB::Red;
  leds[58]= CRGB::Red;
  leds[59]= CRGB::Red;
  leds[60]= CRGB::Red;
  leds[61]= CRGB::Red;
  leds[62]= CRGB::Red;
  leds[63]= CRGB::Red;
  leds[64]= CRGB::Red;
  leds[65]= CRGB::Red;
  leds[66]= CRGB::Red;
  leds[67]= CRGB::Red;
  leds[68]= CRGB::Red;
  leds[69]= CRGB::Red;
  leds[70]= CRGB::Red;
  leds[71]= CRGB::Red;
  leds[72]= CRGB::Red;
  leds[73]= CRGB::Red;
  leds[74]= CRGB::Red;
  leds[75]= CRGB::Red;
  leds[76]= CRGB::Red;
  leds[77]= CRGB::Red;
  leds[78]= CRGB::Red;
  leds[79]= CRGB::Red;
  leds[80]= CRGB::Red;
  leds[81]= CRGB::Red;
  leds[82]= CRGB::Red;
  leds[83]= CRGB::Red;
  leds[84]= CRGB::Red;
  leds[85]= CRGB::Red;
  leds[86]= CRGB::Red;
  leds[87]= CRGB::Red;
  leds[88]= CRGB::Red;
  leds[89]= CRGB::Red;
  leds[90]= CRGB::Red;
  leds[91]= CRGB::Red;
  leds[92]= CRGB::Red;
  leds[93]= CRGB::Red;
  leds[94]= CRGB::Red;
  leds[95]= CRGB::Red;
  leds[96]= CRGB::Red;
  leds[97]= CRGB::Red;
  leds[98]= CRGB::Red;
  leds[99]= CRGB::Red;
  leds[100]= CRGB::Red;
  leds[101]= CRGB::Red;
  leds[102]= CRGB::Red;
  leds[103]= CRGB::Red;
  leds[104]= CRGB::Red;
  leds[105]= CRGB::Red;
  leds[106]= CRGB::Red;
  leds[107]= CRGB::Red;
  leds[108]= CRGB::Red;
  leds[109]= CRGB::Red;
  leds[110]= CRGB::Red;
  leds[111]= CRGB::Red;
  leds[112]= CRGB::Red;
  leds[113]= CRGB::Red;
  leds[114]= CRGB::Red;
  leds[115]= CRGB::Red;
  leds[116]= CRGB::Red;
  leds[117]= CRGB::Red;
  leds[118]= CRGB::Red;
  leds[119]= CRGB::Red;
  leds[120]= CRGB::Red;
  leds[121]= CRGB::Red;
  leds[122]= CRGB::Red;
  leds[123]= CRGB::Red;
  leds[124]= CRGB::Red;
  leds[125]= CRGB::Red;
  leds[126]= CRGB::Red;
  leds[127]= CRGB::Red;
  leds[128]= CRGB::Red;
  leds[129]= CRGB::Red;
  leds[130]= CRGB::Red;
  FastLED.show();
}



