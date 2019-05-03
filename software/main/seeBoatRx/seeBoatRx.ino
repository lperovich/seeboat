// Feather9x_RX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (receiver)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_TX

//Edits by Rod 2017
//Has a data structure to pull in all the data that SeaBoat is sending

#include <SPI.h>
#include <RH_RF95.h>

/* for feather32u4
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
*/

/* for feather m0 */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3


/* for shield 
#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 7
*/


/* for ESP w/featherwing 
#define RFM95_CS  2    // "E"
#define RFM95_RST 16   // "D"
#define RFM95_INT 15   // "B"
*/

/* Feather 32u4 w/wing
#define RFM95_RST     11   // "A"
#define RFM95_CS      10   // "B"
#define RFM95_INT     2    // "SDA" (only SDA/SCL/RX/TX have IRQ!)
*/

/* Feather m0 w/wing 
#define RFM95_RST     11   // "A"
#define RFM95_CS      10   // "B"
#define RFM95_INT     6    // "D"
*/

/* Teensy 3.x w/wing 
#define RFM95_RST     9   // "A"
#define RFM95_CS      10   // "B"
#define RFM95_INT     4    // "C"
*/

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Blinky on receipt
#define LED 13

//version with the seeboat data
typedef struct {
  int           deviceID; //store this nodeId
  float GPSlat; //latitude
  float GPSlong;   //longitude
  //send all the time parts individually; strings have been annoying to send
  int GPShour;
  int GPSmin;
  int GPSsec;
  int GPSms;
  float tempVal; //temperature value
  float condVal;
  float pHVal;
  float milliIrradiance;
} Payload;
Payload theData;




////////////////////////////////////////////////////////////////
void setup() 
{
radioSetup();
}

void loop()
{
radioReceive();

}

///////////////////////////////////////////////////////// FUNCTIONS
void radioSetup(){
    pinMode(LED, OUTPUT);     
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  //take this serial line out if you're not necessarily connected to the computer!
  //while (!Serial);
  Serial.begin(9600);
  delay(100);

  Serial.println("Feather LoRa RX Test!");
  
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);

//Print the column headers
Serial.println("deviceID, GPSlat, GPSlong, GPShour, GPSmin, GPSsec, GPSms, tempVal");

}

/////////////////////////////////////////////////////////
void radioReceive(){
    if (rf95.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    
    if (rf95.recv(buf, &len))
    {
      //from moteino code
      theData = *(Payload*)buf;
      //theData = *(Payload*)radio.DATA; //assume radio.DATA actually contains our struct and not something else
      Serial.print(" deviceID=");
      Serial.print(theData.deviceID);
      Serial.print(",");
      Serial.print(" GPSlat=");
      Serial.print(theData.GPSlat,7);
      Serial.print(",");
      Serial.print(" GPSlong=");
      Serial.print(-theData.GPSlong,7);
      Serial.print(",");
      Serial.print(" GPShour=");
      Serial.print(theData.GPShour);  
      Serial.print(",");
      Serial.print(" GPSmin=");
      Serial.print(theData.GPSmin);  
      Serial.print(",");
      Serial.print(" GPSsec=");
      Serial.print(theData.GPSsec);  
      Serial.print(",");
      Serial.print(" GPSms=");
      Serial.print(theData.GPSms);  
      Serial.print(",");
      Serial.print(" tempVal=");
      Serial.print(theData.tempVal);
      Serial.print(",");
      Serial.print(" condVal=");
      Serial.print(theData.condVal);
      Serial.print(",");
      Serial.print(" pHVal=");
      Serial.print(theData.pHVal);
      Serial.print(",");
      Serial.print(" milliIrradiance=");
      Serial.print(theData.milliIrradiance);
      Serial.println();
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
}

