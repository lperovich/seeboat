//Code notation by Talia and Laura July 2019
//Make sure to plug in a battery (don't just connect by USB to the computer) to make sure the LEDs are sufficiently powered
// Simple strand test for Adafruit Dot Star RGB LED strip.

/////////////////////////////////////// DEFINE THE VARIABLES
#include <Adafruit_DotStar.h>
#include <SPI.h>         

// Number of LEDs in strip
#define NUMPIXELS 44 

// Here's how to control the LEDs from any two pins:
#define DATAPIN    13
#define CLOCKPIN   11
Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);

//YOU CAN ADD NEW COLORS HERE IN TWO WAYS
// in one way you look up the hex code of the color here (https://html-color-codes.info/) and write it out
//// NOTE: OUR STRIPS ARE #GGRRBB not #RRGGBB, so the order will be a little different //////
uint32_t green = 0xFF0000;      // green using hex 
uint32_t blue = 0x0000FF;      // blue using hex
uint32_t yellow = 0xFFFF00;      // yellow using hex
uint32_t white = 0xFFFFFF;        // white using hex
//Create a new color using this template:
//uint32_t white = 0xFFFFFF;        // white using hex

// in the second way, you tell the LEDs how much of each color you want to use
// each of the three numbers in this line can be any number between 0 and 255
// one number represents how much red, one represents how much green, and one represents how much blue 
uint32_t red = strip.Color(0, 255, 0);    // red using rgb
uint32_t orange = strip.Color(50, 255, 0);    // orange using rgb
uint32_t green2 = strip.Color(255, 0, 0);    // green using rgb
uint32_t purple3 = strip.Color(0, 255, 255);   // purple using rgb
uint32_t purple = strip.Color(0, 255, 255);   // purple using rgb
//Create a new color using this template:
//uint32_t purple = strip.Color(0, 255, 255);   // purple using rgb

uint32_t yellow2 = strip.Color(255, 255, 0); 
uint32_t purple2 = strip.Color(9, 89, 168); 
uint32_t darkBlue = strip.Color(23, 0, 172); 

/////////////////////////////////////// SET EVERYTHING UP
void setup() {
  //Get the lights ready to turn out
  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
}



/////////////////////////////////////// RUN THE CODE
void loop() {
  //Put the color that you want for the LEDs here, based on the color names that you created above. The LED strand will turn this color
  strip.fill(red);
  strip.show();
  delay(1000);
  strip.fill(orange);
  strip.show();
  delay(1000);
  strip.fill(yellow2);
  strip.show();
  delay(1000);
   strip.fill(green2);
  strip.show();
  delay(1000);
  strip.fill(darkBlue);
  strip.show();
  delay(1000);
  strip.fill(purple2);
  strip.show();
  delay(1000);

  
 //you can create two consecutive colors using delays
 //delay(1000);
 //strip.fill(purple);
 //strip.show();
 
}
