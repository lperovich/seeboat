//Code by Laura/Talia September 2019
//Alternative route to SeeBoat with conductivity--uses a SD data logging to keep track of position so the data can be remerged later
//Combines the SD card datalogger example code (Tom Igoe) with the conductivity logging code from the SeeBoat code
//Try to write the data to the card in a way that will be 'easy' to combine with the measurement data using R or some statistical software
//Added 12 bit instead of 10 December 16th, 2019

//LP: changed to sine wave input off A0 instead of square wave input off D5 Feb 2020
//Using code from this (attachment post by by BDL (Wed Jul 17, 2019 10:34 am): https://forums.adafruit.com/viewtopic.php?f=57&t=153490&start=15
//LP: changed to go back to 10 bit and include sine waveinput to probe Feb 2020

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

//remove this since we're getting a sine wave off A0 now
//int power = 5;              //50% square wave (yellow wire) connected to pin 5 on the feather
int sensor = A1;             //final voltage (purple wire)
float val = 0.0;
float conductivity;
double voltage;

//Sine wave stuff
#define const_pwr       // ifndef get constant (single cycle per frequency) chirp.  
                        // ifdef  get constant power per fft bin
volatile uint16_t sintable1[32];
volatile uint32_t freq;

//what it sweeps to and from. Put this at 10K constantly for our case
//uint32_t basefreq = 100;          // min freq
uint32_t basefreq = 10000;          // min freq
uint32_t maxfreq  = 10000;        // max freq
//uint32_t maxfreq  = 25000;        // max freq

typedef struct                                                                    // DMAC descriptor structure
{
  uint16_t btctrl;
  uint16_t btcnt;
  uint32_t srcaddr;
  uint32_t dstaddr;
  uint32_t descaddr;
} dmacdescriptor ;

volatile dmacdescriptor wrb[12] __attribute__ ((aligned (16)));                   // Write-back DMAC descriptors
volatile dmacdescriptor descriptor_section[12] __attribute__ ((aligned (16)));    // DMAC channel descriptors
dmacdescriptor descriptor __attribute__ ((aligned (16)));                         // Place holder descriptor




///////////////////////////////////////////////////////////////////////////////////////   SETUP

void setup() {

  delay(10000);
  //SD setup
  SDsetup();
  SDprep();

  Wire.begin();
 
  init_AT30TS750A();

  Serial.begin(9600);
//  pinMode(power, OUTPUT);
  pinMode(sensor, INPUT);

  //don't do 12 bit because maybe the sine wave needs 10
  //we can do 12 bit on the feather to get better data
  //analogReadResolution(12);

//Sine wave stuff
  // ================================================================================================================
  #ifdef serial
//  Serial.begin(2000000);                                           // Comment this line and next for full performance
//  while(!Serial);
  #endif
  // ================================================================================================================
  
  for (uint16_t i = 0; i < 32; i++)                                // Calculate the sine table with 32 entries
  {
    sintable1[i] = (uint16_t)((sinf(2 * PI * (float)i / 32) * 511) + 512);
  }

  analogWriteResolution(10);                                        // Set the DAC's resolution to 10-bits
  analogWrite(A0, 0);                                               // Initialise the DAC

  //===============
  // Initialize DMA
  //===============
  DMAC->BASEADDR.reg = (uint32_t)descriptor_section;                // Set the descriptor section base address
  DMAC->WRBADDR.reg = (uint32_t)wrb;                                // Set the write-back descriptor base adddress
  DMAC->CTRL.reg = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xf);      // Enable the DMAC and priority levels

  DMAC->CHID.reg = DMAC_CHID_ID(0);                                 // Select DMAC channel 0
  DMAC->CHINTENSET.reg = DMAC_CHINTENSET_SUSP;                      // Enable suspend channel interrupts on each channel
  DMAC->CHCTRLB.reg = DMAC_CHCTRLB_LVL(0) |                         // Set DMAC priority to level 0 (lowest)
                      DMAC_CHCTRLB_TRIGSRC(TCC0_DMAC_ID_OVF) |      // Trigger on timer TCC0 overflow
                      DMAC_CHCTRLB_TRIGACT_BEAT;                    // Trigger every beat
  descriptor.descaddr = (uint32_t)&descriptor_section[0];                  // Set up a circular descriptor
  descriptor.srcaddr = (uint32_t)&sintable1[0] + 32 * sizeof(uint16_t);    // Read the current value in the sine table
  descriptor.dstaddr = (uint32_t)&DAC->DATA.reg;                           // Copy it into the DAC data register
  descriptor.btcnt = 32;                                                   // This takes the number of sine table entries = 256 beats
  descriptor.btctrl = DMAC_BTCTRL_BLOCKACT_SUSPEND |                // Suspend DMAC channel at end of block transfer
                      DMAC_BTCTRL_BEATSIZE_HWORD |                  // Set the beat size to 16-bits (Half Word)
                      DMAC_BTCTRL_SRCINC |                          // Increment the source address every beat
                      DMAC_BTCTRL_VALID;                            // Flag the descriptor as valid
  memcpy((void*)&descriptor_section[0], &descriptor, sizeof(dmacdescriptor));  // Copy to the channel 0 descriptor  
 
  NVIC_SetPriority(DMAC_IRQn, 0);           // Set the Nested Vector Interrupt Controller (NVIC) priority for the DMAC to 0 (highest) 
  NVIC_EnableIRQ(DMAC_IRQn);                // Connect the DMAC to the Nested Vector Interrupt Controller (NVIC)

  //=============================
  // Initialize clocks and timer
  //=============================
  
  GCLK->GENDIV.reg = GCLK_GENDIV_DIV(1) |          // Divide the 48MHz clock source by divisor 1: 48MHz/1=48MHz
                     GCLK_GENDIV_ID(4);            // Select Generic Clock (GCLK) 4
  while (GCLK->STATUS.bit.SYNCBUSY);               // Wait for synchronization

  GCLK->GENCTRL.reg = GCLK_GENCTRL_IDC |           // Set the duty cycle to 50/50 HIGH/LOW
                      GCLK_GENCTRL_GENEN |         // Enable GCLK4
                      GCLK_GENCTRL_SRC_DFLL48M |   // Set the 48MHz clock source
                      GCLK_GENCTRL_ID(4);          // Select GCLK4
  while (GCLK->STATUS.bit.SYNCBUSY);               // Wait for synchronization

  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |         // Enable GCLK4 to TCC0 and TCC1
                      GCLK_CLKCTRL_GEN_GCLK4 |     // Select GCLK4
                      GCLK_CLKCTRL_ID_TCC0_TCC1;   // Feed GCLK4 to TCC0 and TCC1
  while (GCLK->STATUS.bit.SYNCBUSY);               // Wait for synchronization

  TCC0->WAVE.reg = TCC_WAVE_WAVEGEN_NFRQ;          // Setup TCC0 in Normal Frequency (NFRQ) mode
  while (TCC0->SYNCBUSY.bit.WAVE);                 // Wait for synchronization
  
  freq = basefreq;                                 // set freq to minimum
                 
  #ifdef serial
  Serial.print("Start period is: ");  Serial.println(48000000/(freq *32) - 1);
  #endif
  TCC0->PER.reg = 48000000/(freq *32) - 1;
  
  while(TCC0->SYNCBUSY.bit.PER);                   // Wait for synchronization
 
  TCC0->CTRLA.reg = TCC_CTRLA_PRESCALER_DIV1;      // Set the TCC0 prescaler to 1 giving 48MHz (20.83ns) timer tick
  TCC0->CTRLA.bit.ENABLE = 1;                      // Enable the TCC0 output
  while (TCC0->SYNCBUSY.bit.ENABLE);               // Wait for synchronization
 
  DMAC->CHID.reg = DMAC_CHID_ID(0);                // Select DMAC channel 
  DMAC->CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE;        // Enable DMAC channel 

}

///////////////////////////////////////////////////////////////////////////////////////   LOOP

void loop() {
  int i = 0;
  //skip the tone part since we don't want the square wave now
  while(i<720) {
//    tone(power,10000);
  val = avg(sensor);
  
  //Adjusted 12.16/19 for 12-bit instead of 10, i.e. up to 4095
  //analog read goes from 0-1023; our range of voltage goes from 0 to 3.3, so scale things accordingly to get a voltage value
  //But we're working from half ground, so the lowest we'll actually every read off of analogue read is half of 1023
  //map this into the full voltage range (e.g. 1023/2 should be zero volts; 1023 should go to 3.3 volts)
  voltage = mapFloat(val, 1023/2, 1023, 0, 3.3);
//  voltage = mapFloat(val, 4095/2, 4095, 0, 3.3);

  //NOTE: the conductivity code in the SeeBoat Feather code also adjust for the temperature (this impacts conductivity)

  //convert voltage to conductivity (microS)
  conductivity = voltToCondRes12(voltage); //for resistor = 1.2 kohm
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

  Serial.print(voltage,4);
  Serial.print(", ");
  Serial.println(temperature);

  SDwrite();
  i = i+1;
  delay(10000);
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


  //averages data
  long avg(int pin) {
  #define SAMPLES 10
//  #define SAMPLES 100
  float val = 0;
  for(unsigned int j=0; j<SAMPLES; j++) 
    val = val + analogRead(sensor);
  return val / SAMPLES;
}

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
    String dataString = "voltage (V), temperature (F)";
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
    dataFile.print(voltage,4);
    dataFile.print(", ");

    dataFile.println(temperature, 4);

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
 
/////////////////////////////////////////////////////////////////////////////////
void DMAC_Handler()
{
  //static uint16_t period = 14999;                       // Initialise 100Hz sine wave, 32 samples: 48MHz / (100 * 32) - 1
  static uint16_t period = 2999;                          // Corresponds to 500 Hz, gets overwritten
  static uint16_t count  = 1;                             // BDL number of periods of each sine wave (constant power case)
  static uint16_t idx = 0;                                // index counter
  
  DMAC->CHID.reg = DMAC_CHID_ID(DMAC->INTPEND.bit.ID);    // Find the DMAC channel generating the interrupt
  descriptor_section[0].btctrl &= ~DMAC_BTCTRL_VALID;     // Disable the descriptor   
  TCC0->PERB.reg = period;                                // 2kHz sine wave, 128 samples: 48MHz / (2000 * 128) - 1
  while(TCC0->SYNCBUSY.bit.PERB);                         // Wait for synchronization
  
  #ifdef serial
  Serial.print("idx  :"); Serial.println(idx);
  Serial.print("count:"); Serial.println(count);
  #endif
  
  #ifdef const_pwr
  if (idx >= count)                                       // Have satisfied the constant power per frequency condition
  {
    idx = 0;                                              // reset the index   
    freq += 100;                                          // we want to sweep frequency
    freq  = freq > (maxfreq+1) ? basefreq : freq;                    // reset frequency if f > 250000 Hz
    period = 48000000/(freq * 32) -1;                     // then calc out the best fit period
    //Serial.print("freq  :"); Serial.println(freq);      // have to comment out serial to run at speed
    //Serial.print("period:"); Serial.println(period);   
    count = uint16_t (freq/basefreq);                          // reset the count.  count = desired_freq/base_freq
  }
  idx += 1;                                               // increment the index                           
  #endif
  
  #ifndef const_pwr
  freq += 100;                                            // we want to sweep frequency
  freq  = freq > (maxfreq+1) ? basefreq : freq;                      // reset frequency if f > 250000 Hz
  period = 48000000/(freq * 32) -1;                       // then calc out the best fit period
  #endif
  
  descriptor_section[0].btctrl |= DMAC_BTCTRL_VALID;      // Enable the descriptor                       
  DMAC->CHCTRLB.reg |= DMAC_CHCTRLB_CMD_RESUME;           // Resume the DMAC channel
  DMAC->CHINTFLAG.bit.SUSP = 1;                           // Clear the DMAC channel suspend (SUSP) interrupt flag  
}
