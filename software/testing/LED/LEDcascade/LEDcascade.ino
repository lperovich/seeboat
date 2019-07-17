//Modified by Laura Perovich 7/3/19 to have the correct pins for the SeeBoat breakout board control board pins
//Make sure to plug in a battery (don't just connect by USB to the computer) to make sure the LEDs are sufficiently powered

// Simple strand test for Adafruit Dot Star RGB LED strip.

#include <Adafruit_DotStar.h>
#include <SPI.h>         

// Put the number of LEDs that are in the strand (or that you want to use in the strand) here:
#define NUMPIXELS 44 

//
// Put the pin numbers of the LED strand here. For the SeeBoat feather breakout, the pins should be 13 and 11:
#define DATAPIN    13
#define CLOCKPIN   11
Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);
// The last parameter is optional -- this is the color data order of the
// DotStar strip, which has changed over time in different production runs.
// Your code just uses R,G,B colors, the library then reassigns as needed.
// Default is DOTSTAR_BRG, so change this if you have an earlier strip.

// Hardware SPI is a little faster, but must be wired to specific pins
// (Arduino Uno = pin 11 for data, 13 for clock, other boards are different).
//Adafruit_DotStar strip(NUMPIXELS, DOTSTAR_BRG);

void setup() {

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
}

// Runs 10 LEDs at a time along strip, cycling through red, green and blue.
// This requires about 200 mA for all the 'on' pixels + 1 mA per 'off' pixel.

int      head  = 0, tail = -10; // Index of first 'on' and 'off' pixels that will cascade around the LED strand
uint32_t color = 0xFF0000;      // 'On' color (starts red). This is a hex color. You can see more here: https://html-color-codes.info/

//FF0000 is red; 00FF00 is green; 0000FF is blue

void loop() {

  strip.setPixelColor(head, color); // 'On' pixel at head
  strip.setPixelColor(tail, 0);     // 'Off' pixel at tail
  strip.show();                     // Refresh strip
  delay(20);                        // Pause 20 milliseconds (~50 FPS)

  if(++head >= NUMPIXELS) {         // Increment head index.  Off end of strip? ++ means add one to that variable
    head = 0;                       //  Yes, reset head index to start
    if((color >>= 8) == 0)          //  Next color (R->G->B) ... past blue now? >> is a rightward shift operator (moving the FFs to the right by 8 positions)  
      color = 0xFF0000;             //   Yes, reset to red
  }
  if(++tail >= NUMPIXELS) tail = 0; // Increment, reset tail index
}
