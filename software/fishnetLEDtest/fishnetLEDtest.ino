/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly. 
  This example code is in the public domain.
 */

#include <SPI.h>
#include "I2C.h"
#include "Time.h" //for keeping track of time
#include "SD.h" //for writing out to csv
#include "SoftPWM.h"


// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
//int led = 13;
int outputRed = 6;
int outputGreen = 5;
int outputBlue = 4;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
//  pinMode(led, OUTPUT);     
  pinMode(outputRed, OUTPUT);     
//  pinMode(outputBlue, OUTPUT);     
  pinMode(outputGreen, OUTPUT);

SoftPWMBegin();
//SOFTPWM_NORMAL, SOFTPWM_INVERTED
SoftPWMSet(outputBlue, 0);
SoftPWMSetFadeTime(outputBlue, 1000, 1000);
  
}

//////////////////////////////////////////////////

// the loop routine runs over and over again forever:
void loop() {
//  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)

  int red = 0;
  int green = 0;
  int blue =0;

  analogWrite(outputRed,0); //Red pin 
  analogWrite(outputGreen,0); //green pin


  SoftPWMSet(0, outputBlue);
  delay(5000);
//  SoftPWMSet(50, outputBlue);
//  delay(500);
//  SoftPWMSet(100, outputBlue);
//  delay(500);
//  SoftPWMSet(150, outputBlue);
//  delay(500);
//  SoftPWMSet(200, outputBlue);
//  delay(500);
  SoftPWMSet(255, outputBlue);
  delay(5000);


  /*
  //just red increase in intensity (off to on)
  for (red=0; red<256; red++){
  setLedColor(red, 0, 0);
  delay(100);
}
setLedColor(0, 0, 0);
delay(10000);
*/
/*
//Need to do this one in software?  It doesn't fade, might be on the non-PWM pin
 //just blue increase in intensity (off to on)
  for (blue=0; blue<256; blue++){
  setLedColor(0, 0, blue);
  delay(100);
}
setLedColor(0, 0, 0);
delay(10000);
*/
/*
  //just green increase in intensity (off to on)
  for (green=0; green<256; green++){
  setLedColor(0, green, 0);
  delay(100);
}
setLedColor(255, 255, 255);
delay(10000);
setLedColor(0, 0, 0);
delay(10000);
*/

/*
  //all increase in intensity (off to on)
  for (red=0; red<256; red++){
  setLedColor(red, red, red);
  delay(100);
}
setLedColor(255, 255, 255);
delay(10000);
*/

}


//////////////////////////////////////////////////

//---------------------------------------------------------------------------
//Sets the current color for the RGB LEDs (single LED setup)
void setLedColor(int red, int green, int blue) {
  //Note that we are reducing 1/4 the intensity for the green and blue components because 
  //  the red one is too dim on my LED. You may want to adjust that. <--NOTE FROM PAST CODE; WHERE DOES THIS HAPPEN?
  analogWrite(outputRed,red); //Red pin 
  analogWrite(outputGreen,green); //green pin
  //SoftwarePWM the blue pin because it's a weirdo non-hardware one
  SoftPWMSet(blue, outputBlue);
//  analogWrite(outputBlue,blue); //blue pin 

  //for debugging
  //showColors(red, green, blue);
}

