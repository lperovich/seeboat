//LP August 2015
//modified from the AT30TS750A_multiple code
//String together and read from more than one temp sensing board.
//(note: do I need to worry about having all those resisters and capacitors in there???  how does this work??)
//added SD card writing to record the data, as by Datalogger sketch in examples->SD
//see: https://learn.adafruit.com/adafruit-micro-sd-breakout-board-card-tutorial/ for wiring etc
//can get i2c addresses using the i2cScanner arduino sketch
/*
Originals
 Website:              http://www.rheingoldheavy.com/mcp7940-and-at30ts750a-tutorial-05-temp-and-time-and-errors
 MCP7940N Datasheet:   http://ww1.microchip.com/downloads/en/DeviceDoc/20005010F.pdf
 AT30TS750A Datasheet: http://www.atmel.com/Images/Atmel-8855-DTS-AT30TS750A-Datasheet.pdf

https://rheingoldheavy.com/at30ts750a-tutorial-02-retrieving-temperature-value/
https://rheingoldheavy.com/mcp7940-and-at30ts750a-tutorial-05-time-and-temp-and-errors/
https://rheingoldheavy.com/mcp7940-tutorial-02-setting-and-getting-time/

Making the board:wiring diagram for similar breakout board
Think about how we handle the resistors here (and capacitance).  Seems like this is an issue on the lines...esp for chained sensors
http://rheingoldheavy.com/wp-content/uploads/2015/02/Schematic_AT30TS750A.png

 */

//TO DO:
//GPS module
//change .txt to .csv and make sure it is openable as a csv
//some sort of file management?  Press a button to start writing to a new file?  something else?
//add a time/date stamp to the file name in the setup(maybe?? tried and this was a huge pain, perhaps unnecessary)
//FIX .0625 => .625---DONE
//FIX: time loops after 24 hrs
//ADD: LEDs---DONE

/////////////////////////////////////////////////////////////////////////////

#include <SPI.h>
#include "I2C.h"
#include "Time.h" //for keeping track of time
#include "SD.h" //for writing out to csv


//const byte   AT30TS750_I2C        = 0x48;  // I2C Address for the temperature sensor
//as by data sheet--1001 000 (device ID and then all pins connected to ground)
//1001 111 = 0x4f (all pins connected to power)
//CHANGE THIS FOR DIFFERENT PIN CONFIGURATIONS!!
//1001 A2 A1 A0

// ********* SELECT THE I2C ADDRESS THAT CORRESPOND TO THE BREAKOUT BOARDS ******************
// update later code accordingly

//const byte   AT30TS750_I2Ca        = 0x4F; //1001000
//const byte   AT30TS750_I2Cb        = 0x48; //1001001
const byte   AT30TS750_I2Cc        = 0x4B; //1001000
//const byte   AT30TS750_I2Cd        = 0x49; //1001001

const byte   AT30TS750_REG_TEMP   = 0x00;  // Register Address: Temperature Value 0x00
const byte   AT30TS750_REG_CONFIG = 0x01;  // Register Address: Temperature sensor configuration
const byte   AT30TS750_REG_TEMP_RESOLUTION = 0x60; //0x00 for 9 bit; 0x60 for 12-bit

byte         timeStamp[7];                 // Byte array holding a full time stamp.
unsigned int errorCount           = 0;     // Large integer to hold a running error count
byte         errorStatus          = 0;     // Byte value to hold any returned I2C error code

byte temperature[2]; //Byte array holding the temperature; use more or less of this depending on precision.  uint_8 for whole number
//temp oC*10 for each sensor
int tempC1decA = 0;
int tempC1decB = 0;
int tempC1decC = 0;
int tempC1decD = 0;


//SD card pin number (7 for the fishnet board!!)
//const int chipSelect = 10;
const int chipSelect = 7;


//LED output pins
//these have to be PWM
//CHECK CHECK CHECK double check the colors are wired right!!!!
const int outputRed = 6;
const int outputGreen = 5;
const int outputBlue = 4;

//Set the color range boundaries (in oF*10)
//const float lowReading = 75; //77
//const float highReading = 90; //85
const int lowReading1dec = 580; //58
const int highReading1dec = 850; //85

//range for this time frame over the past month for EPA buoy: 68-83

/////////////////////////////////////////////////////////////////////////////
void setup()
{
  temperature[0] = 0;
  temperature[1] = 0;
  
  //Get the lights ready to go
  lightSetup();
  ledTest();
  Serial.begin (9600);

  delay(100);

  I2c.begin();       // Initialize the I2C library
//  I2c.pullup(0);      // Disable the internal pullup resistors
  I2c.setSpeed(0);      // Enable 100kHz I2C Bus Speed
  I2c.timeOut(250);    // Set a 250ms timeout before the bus resets

//Set-up the SD card and write a header to it

  SDsetup();
  File dataFile = SD.open("datalog.csv", FILE_WRITE);
  SDwriteCard(dataFile, "ID, Time, Temperature");

  
  // Initialize the Temp Sensor configuration
  //PICK THE ONES YOU USE
//  init_AT30TS750(AT30TS750_I2Ca);
//  init_AT30TS750(AT30TS750_I2Cb);
  init_AT30TS750(AT30TS750_I2Cc);
//  init_AT30TS750(AT30TS750_I2Cd);


  //Get a set of initial readings so we know where we're at
  //PICK THE ONES YOU USE
  
//  for (int j=0; j<5; j++){
  //getReadingLong(AT30TS750_I2Ca);
  //getReadingLong(AT30TS750_I2Cb);
  //getReadingLong(AT30TS750_I2Cc);
  //getReadingLong(AT30TS750_I2Cd);
  //delay(500);
 // }


  //Tell the human to get ready for the temperature changing
  //startExperiment();

  //delay(500);

  //Restart the time
  setTime(0);
}


void loop()
{
 //Get the readings
 //PICK THE ONES YOU USE
//  tempC1decA =  getReading(AT30TS750_I2Ca);
//  tempC1decB =  getReading(AT30TS750_I2Cb);
  tempC1decC =  getReading(AT30TS750_I2Cc);
// tempC1decD =  getReading(AT30TS750_I2Cd);

  //Change the lights if you so choose!  all at once can get confusing looking
  //tempC1decToColor(tempC1decA);
  //tempC1decToColor(tempC1decB);
  tempC1decToColor(tempC1decC);
//  tempC1decToColor(tempC1decD);

//Wait a little bit betweeen measurements...
  delay(100);
}


