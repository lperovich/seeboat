//Code by Laura September 2019
//Alternative route to SeeBoat with GPS--uses a SD data logging to keep track of position so the data can be remerged later
//Combines the SD card datalogger example code (Tom Igoe) with the GPS logging code from the SeeBoat code
//Try to write the data to the card in a way that will be 'easy' to combine with the measurement data using R or some statistical software

/*
  SD card datalogger
 * analog sensors on analog ins 0, 1, and 2
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

Plus the Adafruit GPS featherwing stacked on top with the antenna extender
 */
///////////////////////////////////////////////////////////////////////////////////////

#include <SPI.h>
#include <SD.h>
#include <Adafruit_GPS.h>

//GPS stuff
//Adafruit_GPS GPS(&Serial1);
#define GPSSerial Serial1
// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);
// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false
//#define GPSECHO true

//SDcard variable
const int chipSelect = 4;
String filename = "";

//Attempt to make a new file for each run...not working, I think because of char filename that needs to be string?
char filenameCh[50];
int randomNumber;

//how often we get GPS data in milliseconds
int howOftenMS = 1000;

//GPS variables
double GPSlat;
double GPSlong;
int GPShour;
int GPSmin;
int GPSsec;
int GPSms;
int GPSday;
int GPSmonth;
int GPSyear;
uint32_t timer = millis();


///////////////////////////////////////////////////////////////////////////////////////   SETUP

void setup() {

  //SD setup
  SDsetup();
  SDprep();

  //GPS start up
  GPSsetup();

}

///////////////////////////////////////////////////////////////////////////////////////   LOOP

void loop() {
 // Serial.println(filename);
  GPSread();
  GPSgetLocTime();
  SDwrite();
//  Serial.println(filename);
//  Serial.println(GPS.day, DEC);
//  Serial.println(GPSday, DEC);

}



/////////////////////////////////////////////////////////////////////////////////////
////////////////////////////      FUNCTIONS      ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
void GPSsetup() {
  //while (!Serial);  // uncomment to have the sketch wait until Serial is ready
  
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
  Serial.println("Adafruit GPS library basic test!");
     
  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz
     
  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(100);
  
  // Ask for firmware version
  GPSSerial.println(PMTK_Q_RELEASE);
  //delay to try to get a fix
}

/////////////////////////////////////////////////////////////////////////////////////////////
void GPSread(){
    // read data from the GPS in the 'main loop'
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!

  if (GPSECHO)
    if (c) Serial.print(c);
    
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    //Serial.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }
  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis()) timer = millis();

}
     
//////////////////////////////////////////////////////////////////////////////
//Function to convert adafruit lat or long values into double that are more useable
//use this for the GPS values that go in the data array
//http://arduinodev.woofex.net/2013/02/06/adafruit_gps_forma/
 
// converts lat/long from Adafruit
// degree-minute format to decimal-degrees
double convertDegMinToDecDeg (float degMin) {
  double min = 0.0;
  double decDeg = 0.0;
 
  //get the minutes, fmod() requires double
  min = fmod((double)degMin, 100.0);
 
  //rebuild coordinates in decimal degrees
  degMin = (int) ( degMin / 100 );
  decDeg = degMin + ( min / 60 );
 
  return decDeg;

}
/*
example
printf(GPS.latitude);
4939.4150
printf(convertDegMinToDecDeg(GPS.latitude));
49.656917
*/

//////////////////////////////////////////////////////////////////////////////
void GPSgetLocTime(){
    if (millis() - timer > howOftenMS) {

      timer = millis(); // reset the timer

      GPSlat = convertDegMinToDecDeg(GPS.latitude);
      GPSlong = -convertDegMinToDecDeg(GPS.longitude);

      GPShour = GPS.hour;
      GPSmin = GPS.minute;
      GPSsec = GPS.seconds;
      GPSday = GPS.day;
      GPSmonth = GPS.month;
      GPSyear = GPS.year;

      Serial.print(GPSlat, 5);
      Serial.print(", ");
      Serial.println(GPSlong, 5);
    }
}
//////////////////////////////////////////////////////////////////////////////
void SDsetup(){
  //make sure this is (probably) a new file, by adding a number to it
    randomSeed(analogRead(0));
    randomNumber = random(999);
    sprintf(filenameCh, "datalog%u.csv", randomNumber);
    filename = (String)filenameCh;
//    filename = "datalog.csv";
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
    String dataString = "GPS.lat, GPS.long, date, time";
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
    dataFile.print(GPSlat, 6);
    dataFile.print(", ");

    dataFile.print(GPSlong, 6);
    dataFile.print(", ");

    dataFile.print(GPSmonth, DEC);
    dataFile.print("/");
    dataFile.print(GPSday, DEC);
    dataFile.print("/");
    dataFile.print(GPSyear, DEC);
    dataFile.print(", ");

    dataFile.print(GPShour);
    dataFile.print(":");
    dataFile.print(GPSmin);
    dataFile.print(":");
    dataFile.println(GPSsec);
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

  //////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////

void GPSprintLoc(){
    if (millis() - timer > 2000) {
      timer = millis(); // reset the timer
        Serial.print("Location: ");
      Serial.print(convertDegMinToDecDeg(GPS.latitude), 5);
      //Serial.print(GPS.latitude, 4); 
      //Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.println(convertDegMinToDecDeg(GPS.longitude), 5);
      //Serial.print(GPS.longitude, 4); 
      //Serial.println(GPS.lon);
//      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
//      Serial.print("Angle: "); Serial.println(GPS.angle);
//      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
    }
}
//////////////////////////////////////////////////////////////////////////////
void GPSprintDays(){
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
    Serial.println();

}

//////////////////////////////////////////////////////////////////////////////
//Modified from the flora_parsing code from adafruit tutorial
void GPSprint() // run over and over again
{
  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
    if (c) Serial.print(c);
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    Serial.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }
  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis()) timer = millis();
     
  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) {
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
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
    }
  }
}

//////////////////////////////////////////////////////////////////////////////
//Function for GPS-ing time
//WORK ON GETTING THIS OUT RIGHT
String GPSgettime(){
    // read data from the GPS in the 'main loop'
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  //if (GPSECHO)
    //if (c) Serial.print(c);

    String nowTime;
    nowTime = (String)GPS.hour + "s";
    Serial.print("\nTime: ");
    Serial.print(GPS.hour, DEC); Serial.print(':');
    Serial.print(GPS.minute, DEC); Serial.print(':');
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    Serial.print(nowTime);
    Serial.println(GPS.milliseconds);
    return nowTime;
}
