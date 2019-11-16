/*
  SD card datalogger
 * analog sensors on analog ins 0, 1, and 2
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)
 */
///////////////////////////////////////////////////////////////////////////////////////

#include <SPI.h>
#include <SD.h>
#include "Wire.h"
 
//const int  AT30TS750_I2C =  0x48;    // I2C Address for the temperature sensor
const int  AT30TS750_I2C =  0x4B;    // I2C Address for the temperature sensor

const byte REG_TEMP      =  0x00;    // Register Address: Temperature Value
const byte REG_CONFIG    =  0x01;    // Register Address: Configuration
float temperature;

//SDcard variable
const int chipSelect = 4;
String filename = "";

//Attempt to make a new file for each run...not working, I think because of char filename that needs to be string?
//char filename[50];
int randomNumber;

//how often we get GPS data in milliseconds
int howOftenMS = 1000;

uint32_t timer = millis();

int power = 5;              //50% square wave (yellow wire) connected to pin 5 on the feather
int sensor = A1;             //final voltage (purple wire)
float val = 0.0;
float conductivity;

int frequency = 100;


///////////////////////////////////////////////////////////////////////////////////////   SETUP

void setup() {

  //delay(10000);
  //SD setup
  //SDsetup();
  //SDprep();

  Wire.begin();
 
  init_AT30TS750A();

  Serial.begin(9600);
  pinMode(power, OUTPUT);
  pinMode(sensor, INPUT);
}

///////////////////////////////////////////////////////////////////////////////////////   LOOP

void loop() {
  int i = 0;
  while(i<100) {
    tone(power,frequency);
  val = analogRead(sensor);

  //analog read goes from 0-1023; our range of voltage goes from 0 to 3.3, so scale things accordingly to get a voltage value
  //But we're working from half ground, so the lowest we'll actually every read off of analogue read is half of 1023
  //map this into the full voltage range (e.g. 1023/2 should be zero volts; 1023 should go to 3.3 volts)
  float voltage = mapFloat(val, 1023/2, 1023, 0, 3.3);

  //NOTE: the conductivity code in the SeeBoat Feather code also adjust for the temperature (this impacts conductivity)

  //convert voltage to conductivity (microS)
  conductivity = voltage; // put function for probe
  //float conductivity = voltToCondRes12(voltage); //for resistor = 1.0 kohm

  byte   tempLSByte       = 0;
  byte   tempMSByte       = 0; 
  float  floatTemperature = 0.0000;
 
  Wire.beginTransmission  (AT30TS750_I2C);
  Wire.write              (REG_TEMP);
  byte error = Wire.endTransmission    ();
//  Serial.print(error);
  
  Wire.requestFrom        (AT30TS750_I2C, 2);
  tempMSByte              = Wire.read();
  tempLSByte              = Wire.read() >> 4;

  floatTemperature        = tempMSByte + (tempLSByte * .0625);
  temperature = floatTemperature;

  Serial.print(frequency);
  Serial.print(", ");
  Serial.println(conductivity);

  //SDwrite();
  i = i+1;
  frequency = frequency + 100;
  }
  delay(100000000);
}

/////////////////////////////////////////////////////////////////////////////////////
////////////////////////////      FUNCTIONS      ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////
     
  // map() function except for floats
  float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

  // converts voltage to conductivity for resistor = 1.2 kohm
  float voltToCondRes12(float voltage) {
    return (voltage)*12878 - 819.5; //for A
    //return (voltage - 0.0321)/(0.00009); //for smaller probe
    //return (voltage)*(17698) - 63.105; //for wider probe
}

  // converts voltage to conductivity for resistor = 1 kohm
  float voltToCondRes1(float voltage) {
    return (voltage)*15332 - 768.52; //for A
    //return (voltage - 0.011)/0.0001; //for smaller probe
    //return (voltage)*(21077) + 143.87; //for wider probe
}

//////////////////////////////////////////////////////////////////////////////
void SDsetup(){
  //make sure this is (probably) a new file, by adding a number to it
    randomSeed(analogRead(0));
    randomNumber = random(999);
    //sprintf(filenameCh, "datalog%u.csv", randomNumber);
    //filename = (String)filenameCh;
    filename = "datalog.csv";
    Serial.println(filename);


  // Open serial communications--don't wait for port to open!!
  Serial.begin(9600);
 // while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
 // }

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
}

/////////////////////////////////////////////////////////////////////////////

  void SDprep(){

  //write the header line
    String dataString = "conductivity (microS/cm), temperature (F)";
    File dataFile = SD.open(filename, FILE_WRITE);

    // if the file is available, write to it:
    if (dataFile) {
    dataFile.println(dataString);

    dataFile.close();
    }
      else {
    Serial.println("first error opening file");
  }

  }

/////////////////////////////////////////////////////////////////////////////

  void SDwrite(){
  // make a string for assembling the data to log:
  String dataString = "";

  // read three sensors and append to the string:
/*  for (int analogPin = 0; analogPin < 3; analogPin++) {
    int sensor = analogRead(analogPin);
    dataString += String(sensor);
    if (analogPin < 2) {
      dataString += ",";
    }
  }
*/
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open(filename, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.print(conductivity);
    dataFile.print(", ");

    dataFile.println(temperature);

//    dataFile.print(", ");


    //dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    //Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening file");
  }
  }
