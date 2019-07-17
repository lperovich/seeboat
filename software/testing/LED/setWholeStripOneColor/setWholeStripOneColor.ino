//Make sure to plug in a battery (don't just connect by USB to the computer) to make sure the LEDs are sufficiently powered
// Simple strand test for Adafruit Dot Star RGB LED strip.

#include <Adafruit_DotStar.h>
#include <SPI.h>         

#define NUMPIXELS 44 // Number of LEDs in strip

// Here's how to control the LEDs from any two pins:
#define DATAPIN    13
#define CLOCKPIN   11
Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);

void setup() {

  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
}

/////////////////////////////////////// OUR STRIPS ARE #GGRRBB not #RRGGBB ////////////////////////////////////////////////////////////////////////////////

uint32_t green = 0xFF0000;      // green using hex 
uint32_t blue = 0x0000FF;      // blue using hex
uint32_t yellow = 0xFFFF00;      // yellow using hex
uint32_t white = 0xFFFFFF;        // white using hex

void loop() {

  uint32_t blue = strip.Color(0, 0, 255);   // blue using rgb
  uint32_t red = strip.Color(0, 255, 0);    // red using rgb
  uint32_t green = strip.Color(255, 0, 0);    // green using rgb
  uint32_t purple = strip.Color(0, 255, 255);   // purple using rgb

  strip.fill(purple);
  strip.show();
  
}
