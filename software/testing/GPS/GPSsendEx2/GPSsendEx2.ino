//attempt to read and send GPS data
//code from https://forums.adafruit.com/viewtopic.php?f=31&t=96028
//(wasn't working there)
     
#include <Adafruit_GPS.h>
#include <SPI.h>
#include <RH_RF95.h>

// what's the name of the hardware serial port?
#define GPSSerial Serial1
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 434.0

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
     

#define GPSECHO false

uint32_t timer = millis();

#define PAYLOAD_SIZE 8

struct SEND_DATA_STRUCTURE
{
  int blinks;
  int pause;

  char data[2];
};

//give a name to the group of data
SEND_DATA_STRUCTURE mydata;

//GPS data
  char buf1[8]; //transmit buffer
  float tdata[2]; //sample data
  float junk[2]={0};  //another float array


////////////////////////////////////////////////////////
void setup()
{
  //while (!Serial);  // uncomment to have the sketch wait until Serial is ready
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
  delay(100);
  Serial.println("Adafruit GPS library basic test!");
     
  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  delay(100);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);

  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);


    // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) 
  {
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

  rf95.setTxPower(23, false);

  delay(1000);
  
  // Ask for firmware version
  GPSSerial.println(PMTK_Q_RELEASE);
}
///////////////////////////////////////////////////////
void loop() // run over and over again
{
GPSstuff();

/////////// THE GPS PART









//radioStuff();
/////////// THE RADIO PART



}
//////////////////////////////////////////////////
void GPSstuff(){
  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
    if (c) Serial.print(c);
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) 
  {

    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }
  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis()) 
    timer = millis();
     
  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) 
  {
    timer = millis(); // reset the timer
    Serial.print("\nTime: ");
    Serial.print(GPS.hour, DEC); Serial.print(':');
    Serial.print(GPS.minute, DEC); Serial.print(':');
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);

if (GPS.fix) 
{

//  char buf1[8]; //transmit buffer
//  tdata={GPS.latitudeDegrees,GPS.longitudeDegrees}; //sample data
  tdata[0]=GPS.latitudeDegrees; //sample data
  tdata[1]=GPS.longitudeDegrees; //sample data

//  junk={0};  //another float array

  memcpy(buf1,tdata,sizeof(tdata)); //copy float data to buf[]

  memcpy(junk,buf1,sizeof(buf1)); //copy buf[] to second float array

Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);

      Serial.println("My Location: ");
    Serial.print(junk[0],5);
    Serial.print(", ");
    Serial.println(junk[1],5);


    Serial.println("---------END-----------");

//THE RADIO PART NEEDS TO BE IN THE GPS PART FOR SOME REASON???????
//possibly a timer thing...
//except now it keep always getting the same GPS values, not new ones. :(
      radioStuff();
}
  }
}

//// PUTTING IT IN A FUNCTION SEEMS TO MESS IT ALL UP!!
/////////////////////////////////////////////////
void radioStuff(){

  Serial.println("Sending..."); delay(10);
  //rf95.send((uint8_t *)radiopacket, 20);
  rf95.send((uint8_t *)junk, sizeof(junk));

  Serial.println("Waiting for packet to complete..."); delay(10);
  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  

  Serial.println("Waiting for reply..."); delay(10);
  if (rf95.waitAvailableTimeout(1000))
  { 
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len))
   {
      Serial.print("Got reply: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);    
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
  else
  {
    Serial.println("No reply, is there a listener around?");
  }
  delay(1000);
}

