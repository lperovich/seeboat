/*
 
 A sketch to retrieve the temperature from the AT30TS750A on the 
 Rheingold Heavy I2C and SPI Education Shield.  The chip setup function
 allows the sketch to be easily modified to retrieve the different
 measurement resolutions that the chip allows for in the datasheet.
 
 Website:   http://www.rheingoldheavy.com/retrieving-temperature-value
 Datasheet: http://www.atmel.com/images/atmel-8855-dts-at30ts750a-datasheet.pdf
 
 */
 
 
#include "Wire.h"
 
//const int  AT30TS750_I2C =  0x48;    // I2C Address for the temperature sensor
const int  AT30TS750_I2C =  0x4B;    // I2C Address for the temperature sensor

const byte REG_TEMP      =  0x00;    // Register Address: Temperature Value
const byte REG_CONFIG    =  0x01;    // Register Address: Configuration
 
 
void setup()
{
  Wire.begin();
 
  init_AT30TS750A();
 
  Serial.begin(9600);
 
}
 
void loop()
{
 tempPrint();
}
 ////////////////////////////////////////////////////////
void tempPrint(){
    byte   tempLSByte       = 0;
  byte   tempMSByte       = 0; 
  float  floatTemperature = 0.0000;
 
  Wire.beginTransmission  (AT30TS750_I2C);
  Wire.write              (REG_TEMP);
  Wire.endTransmission    ();
  Wire.requestFrom        (AT30TS750_I2C, 2);
 
  tempMSByte              = Wire.read();
  tempLSByte              = Wire.read() >> 4;
  floatTemperature        = tempMSByte + (tempLSByte * .0625);
 
  /*  DEBUG CODE
  Serial.print    ("Raw Output Byte 01: 0x");
  Serial.println  (tempMSByte, HEX);
  Serial.print    ("Raw Output Byte 02: 0x");
  Serial.println  (tempLSByte, HEX);  
 
  Serial.print    ("Raw Output Value 01: ");
  Serial.println  (tempMSByte, DEC);
  Serial.print    ("Raw Output Value 02: ");
  Serial.println  (tempLSByte * .0625, DEC);  
  */
    
  Serial.print    ("The temperature is... ");
  Serial.print    (floatTemperature, 4);
  Serial.print    ("C, ");
  Serial.print    ((floatTemperature * (1.8)) + 32, 4);
  Serial.println  ("F");
  Serial.println  ();
//  delay           (100);

}
 
 ////////////////////////////////////////////////////////
void init_AT30TS750A() {
 
  /* Change the resolution of the temperature measurement
       0x00 =  9 bit resolution
       0x20 = 10 bit resolution
       0x40 = 11 bit resolution
       0x60 = 12 bit resolution
  */
  
  Wire.beginTransmission (AT30TS750_I2C);  
  Wire.write             (REG_CONFIG);
//  Wire.write             (0x00);            // Set Measurement Resolution Here
  Wire.write             (0x20);            // Set Measurement Resolution Here
  Wire.endTransmission   ();
}
