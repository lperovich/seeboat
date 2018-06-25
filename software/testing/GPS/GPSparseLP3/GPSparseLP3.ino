//#include "I2C.h"
//#include "Time.h" //for keeping track of time
//#include "SD.h" //for writing out to csv
#include <Adafruit_GPS.h> //for GPS module
#include <SoftwareSerial.h> //for GPS module


//GPS set-up; soft serial is pins 8 and 7 for us here (3,2 is another common option)
SoftwareSerial mySerial(8, 7);
Adafruit_GPS GPS(&mySerial);
// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences. 
#define GPSECHO  false
// this keeps track of whether we're using the interrupt; off by default!
boolean usingInterrupt = false;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy


//SD card pin number
const int chipSelect = 10;


void setup()
{

  //want this faster than the GPS speed
  Serial.begin(9600);

//  Serial.begin(115200);
  Serial.println("Adafruit GPS library basic test yo!");

  delay(100);

  SDsetup();
  delay(200);

  File dataFile = SD.open("data.csv", FILE_WRITE);
  SDwriteCard(dataFile, "Time.hr, Time.min, Time.sec, Date");

  delay(500);

  GPSsetup();
  delay(2000);

}

uint32_t timer = millis();

/*
void loop()
{
  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) { 
    timer = millis(); // reset the timer

  GPSget();
  }
  
}
*/





void loop()                     // run over and over again
{
  // in case you are not using the interrupt above, you'll
  // need to 'hand query' the GPS, not suggested :(
  if (! usingInterrupt) {
    // read data from the GPS in the 'main loop'
    char c = GPS.read();
    // if you want to debug, this is a good time to do it!
    if (GPSECHO)
      if (c) Serial.print(c);
  }
  
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
  
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }


  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) { 
    timer = millis(); // reset the timer




  String nowTime =  GPStime();
//    GPSdate();
//    GPSquality();
//    if (GPS.fix) {
//        GPSplace();
//        GPSother();
//    }


  //Print this to the SD card
  File dataFile = SD.open("dataloc.csv", FILE_WRITE);
  SDwriteCard(dataFile, nowTime);


  
//Put a string together to write to the card.  
/*
  String dataString= "";
  dataString+=String(millis());
  dataString +=", ";
  dataString+=String(GPS.hour);
  dataString +=", ";
  dataString+=String(GPStime());
  dataString +=", ";
  dataString +=String(GPSdate());
  dataString +=", ";
  dataString+=String(GPSplace());
  
  //Print this to the SD card
  File dataFile = SD.open("dataloc.csv", FILE_WRITE);
  SDwriteCard(dataFile, dataString);
*/
  
  
  

//it doesn't like delays!!S
//  delay(4000);
}
}

///////////////////////////////////////////
//Functions to use to talk to the SD card and save data
//added SD card writing to record the data, as by Datalogger sketch in examples->SD
//see: https://learn.adafruit.com/adafruit-micro-sd-breakout-board-card-tutorial/ for wiring etc

/////////////////////////////////////////////////////////////////////////////

//function to get the SD card setup and ready to write to
void SDsetup(){
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
}



//Function to write stuff to the SD card
void SDwriteCard(File dataFile, String dataString){
   
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening dataloc.csv");
  } 
}






///////////////////////////////////////////////////////////////////
//function to get the SD card setup and ready to write to
void GPSsetup(){
    // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate

  //interrupt code is the the parsing example...dig in there...
  // the nice thing about this code is you can have a timer0 interrupt go off
  // every 1 millisecond, and read data from the GPS for you. that makes the
  // loop code a heck of a lot easier!
  //useInterrupt(true);
  Serial.println("hey");


}


//function to do something with GPS...
void GPSget(){
  // in case you are not using the interrupt above, you'll
  // need to 'hand query' the GPS, not suggested :(
  if (! usingInterrupt) {
    // read data from the GPS in the 'main loop'
    char c = GPS.read();
    // if you want to debug, this is a good time to do it!
    if (GPSECHO)
      if (c) Serial.print(c);
  }
  
    // if a sentence is received, we can check the checksum, parse it...
    if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
  
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }





/*
    Serial.print("\nTime: ");
    Serial.print(GPS.hour, DEC); Serial.print(':');
    Serial.print(GPS.minute, DEC); Serial.print(':');
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    Serial.println(GPS.milliseconds);
*/

  //Parse and print out some stuff--don't let this printing drag on too long 
  GPStime();
  //GPSquality();
  //GPSplace(GPS);

}


//Function for GPS-ing time
//WORK ON GETTING THIS OUT RIGHT
String GPStime(){
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

//Function for GPS-ing date
String GPSdate(){
    String nowDate;
    nowDate="";
    nowDate += (String)GPS.day; 
    nowDate+= ", ";
    nowDate+= (String)GPS.month;
    nowDate+= ", 20";
    nowDate+= (String)GPS.year;

    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    
    return nowDate;
}

//Function for GPS-ing signal quality
void GPSquality(){
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality); 
}

//Function for GPS-ing location
String GPSplace(){
    String nowPlace;
    nowPlace="";
    

  if (GPS.fix) {
/*
    nowPlace += (String)GPS.latitudeDegrees;
    nowPlace += ", ";
    nowPlace += (String)GPS.longitudeDegrees;
  */         
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", "); 
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
      Serial.print("Location (in degrees, works with Google Maps): ");
      Serial.print(GPS.latitudeDegrees, 4);
      Serial.print(", "); 
      Serial.println(GPS.longitudeDegrees, 4);      
    }
    return nowPlace;
}

//Function for GPS-ing other stuff
void GPSother(){
         if (GPS.fix) {
      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
    }
}



