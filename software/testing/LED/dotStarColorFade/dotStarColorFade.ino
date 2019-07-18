//Laura July 2018
//back-up for color changing board for ArtBoats
//This just fades the stand continuously in case the remote controlling plan doesn't work
// Based on standtest: Simple strand test for Adafruit Dot Star RGB LED strip.

#include <Adafruit_DotStar.h>
#include <SPI.h>         

// Number of LEDs in strip
#define NUMPIXELS 65 
int numLEDs = 65;

// What pins are the LEDs connected to?
#define DATAPIN    13
#define CLOCKPIN   11

//this controls how long the fade will take (higher is longer, max is about 255)
int fadeLength = 255;

//This is where we decide what color the fade will start at and what color it will go it
//Change the numbers to get different colors, see the bottom of this paragraph for different versions
//Some LED strands are wired slightly differently, so we'll have to test to see what colors we get
//the color we're starting at:
int startRed = 255;
int startGreen = 0;
int startBlue = 0;
//the color we want to go to:
int endRed = 255;
int endGreen = 255;
int endBlue = 0;
//We want five different versions; work along the sides of the RGB space
//green to yellow; 0, 0 ,255 to 0, 255, 255
//: 0, 255, 0; 255, 255, 0
//blue to pink by adding red: 0, 0, 255; 255, 0, 255
//255, 0, 0 ;; 255, 255, 0
// : 255. 255. 0 :: 255, 0, 255
//

int redDiff;
int greenDiff;
int blueDiff;
int nowRed;
int nowBlue;
int nowGreen;

int      head  = 0, tail = -15; // Index of first 'on' and 'off' pixels
uint32_t color = 0xFF0000;      // 'On' color (starts red)

Adafruit_DotStar strip = Adafruit_DotStar(

 NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);
// The last parameter is optional -- this is the color data order of the
// DotStar strip, which has changed over time in different production runs.
// Your code just uses R,G,B colors, the library then reassigns as needed.
// Default is DOTSTAR_BRG, so change this if you have an earlier strip.

// Hardware SPI is a little faster, but must be wired to specific pins
// (Arduino Uno = pin 11 for data, 13 for clock, other boards are different).
//Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DOTSTAR_BRG);

////////////////////////////////////////////////////////////////////////////////
//This is what the code will run once so that the LEDs and the rest of the hardware is ready to go

void setup() {

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
}

/////////////////////////////////////////////////////////////////////////////////
//This is what the code will run over and over again

void loop() {
  stripFadeIn();
  stripFadeOut();
}

///////////////////////////////////////////////////////
//These are functions that we're using inside the "loop" part of the code
void stripFadeIn(){
    redDiff = endRed - startRed;
    greenDiff = endGreen - startGreen;
    blueDiff = endBlue - startBlue;

    //Fade in one direction
    for (int i=0; i<fadeLength; i++){
    nowRed = startRed + floor(i*redDiff/fadeLength);
    nowBlue = startBlue + floor(i*blueDiff/fadeLength);
    nowGreen = startGreen + floor(i*greenDiff/fadeLength);
    
      for (int k = 0; k<numLEDs; k++){         
        strip.setPixelColor(k, strip.Color(nowBlue, nowGreen, nowRed));
       }
      strip.show();
      delay(7);
      Serial.print(nowBlue);
      Serial.print("; ");
      Serial.print(nowGreen);
      Serial.print("; ");
      Serial.println(nowRed);

    }
//    fadeCount=0;

}
////////////////////////////////////////////////
void stripFadeOut(){
    redDiff = startRed-endRed;
    greenDiff = startGreen-endGreen;
    blueDiff = startBlue-endBlue;

    //Fade back in the other direction
    for (int i=0; i<fadeLength; i++){
    nowRed = endRed + floor(i*redDiff/fadeLength);
    nowBlue = endBlue + floor(i*blueDiff/fadeLength);
    nowGreen = endGreen + floor(i*greenDiff/fadeLength);
    
      for (int k = 0; k<numLEDs; k++){         
        strip.setPixelColor(k, strip.Color(nowBlue, nowGreen, nowRed));
       }
      strip.show();
      delay(7);
      Serial.print(nowBlue);
      Serial.print("; ");
      Serial.print(nowGreen);
      Serial.print("; ");
      Serial.println(nowRed);
    }
}
