
#include <FastLED.h>

#include <cmath>

#define NUM_STRIPS 2
#define NUM_LEDS_PER_STRIP 560
#define NUM_LEDS NUM_LEDS_PER_STRIP * NUM_STRIPS

CRGB leds[NUM_STRIPS * NUM_LEDS_PER_STRIP];

// For mirroring strips, all the "special" stuff happens just in setup.  We
// just addLeds multiple times, once for each strip
void setup() {
  // tell FastLED there's 560 NEOPIXEL leds on pin 2, starting at index 0 in the led array
  FastLED.addLeds<DOTSTAR, 11, 13, BGR, DATA_RATE_MHZ(1)>(leds, 0, NUM_LEDS_PER_STRIP);

  // tell FastLED there's 560 NEOPIXEL leds on pin 3, starting at index 560 in the led array
  FastLED.addLeds<DOTSTAR, 26, 27, BGR, DATA_RATE_MHZ(1)>(leds, NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
}


void loop() {
  pride();
  //bright();
  //flashbang();
  FastLED.show();
}

void drawLine(int startX, int startY, int endX, int endY) {
  //ensures the line is always drawn from left to right
  if (endX < startX) {
    int interX = startX;
    startX = endX;
    endX = interX;
  }

  float riseY = (endY - startY);
  float runX = (endX - startX);

  //prevent divide by 0 error
  float slope = runX == 0 ? riseY : riseY/runX;

  //draw the line procedurally from left to right
  for(int i = startX; i < endX; i++) {
    //this will turn on each pixel possibly touched by the line in the y direction
    for(int j = 0; j <= ceil(slope); j++) {
      leds[rectToIndex(startX + i, j + floor(startY))] = CRGB::White; 
    }
    startY += slope;
  }
}

int rectToIndex(int boardX, int boardY) {
  // find the index of the LED at Y level(alternates left and right side)
  int interY = (boardY * 56) - 1;
  // calculate the final index based on whether the Y index starts on the right or left which is determined by even or odd respectively
  int finalIndex = (boardY % 2 == 0) ? interY - (56 - boardX) : interY - (boardX - 1);
  return finalIndex;
}

void bright() {
  for(int i = 0; i < NUM_LEDS; i++) {
   leds[i] = CRGB::White;
  }
}

bool t = false;
void flashbang() {
  for(int i = 0; i < NUM_LEDS; i++) {
   leds[i] = t ? CRGB::White : CRGB::Black;
  }
  t = !t;
}

void pride() 
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
 
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    CRGB newcolor = CHSV( hue8, sat8, bri8);
    
    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS-1) - pixelnumber;
    
    nblend( leds[pixelnumber], newcolor, 64);
  }
}
