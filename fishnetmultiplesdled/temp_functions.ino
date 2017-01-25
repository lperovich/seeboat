//Functions to use to get the temperature from the sensors and do something with the data

/////////////////////////////////////////////////////////////////////////////

//Get a reading from the temperature sensors and write it to the card
//Also returns a temperature celcius*10 value that we can use to change the color of the LED
int getReading(byte AT30TS750_I2C){
//address, register, number of bytes; 1 originally, 2 to get more decimal points
//8 bits =  1 byte, 12 bits = 1.5 bytes
//2 is how much to get, temperature is the variable to put it in.
  errorStatus = I2c.read(AT30TS750_I2C, AT30TS750_REG_TEMP, 2, temperature);

  if (errorStatus != 0) I2CError(errorStatus);


//Get out the person readable device ID
//note that we have 4B and 48 which look super similar but aren't!
  String deviceIDval=getDeviceID(AT30TS750_I2C);

  //Get the temp into a real temperature value
  //first 8 are before the decimal (plus one slot for the sign); next 4 are after the decimal; final 4 are junk
  //FIXXXXXX THIS SO .0625 does not go to .625---THEORETICALLY DONE, CHECK THIS!
  
  ////////////////////////********************
  
  //temperature is a byte array
  int tempLeft = temperature[0];
  int tempRight = temperature[1];  

  String timeValStr= getTimerVal();
  
//Put together the other parts of the measurement line--the seconds and the temperature
  //translate the other side of the decimal point to something human readable
  int tempRightDec = tempRightToDecimal(tempRight);
  String dataString= "";
  dataString+=deviceIDval;
  dataString +=", ";
  dataString += timeValStr;
  dataString +=", ";
  dataString+=String(tempLeft);
  dataString+=".";
  //put in the necessary zero so .0625 doesn't get written as .625 because it's an int
  if (tempRightDec<1000) {
  dataString+="0";
  }
  dataString+=String(tempRightDec);
  
  //Print this to the SD card
  File dataFile = SD.open("datalog.csv", FILE_WRITE);
  SDwriteCard(dataFile, dataString);
  //depending on the number of decimal places we may need to make this longer
  delay(100);  

  //Make this into a number we can write out
  //float breaks everything, int can go up to 32,767; unsigned int to twice that
  //it would be good for us to get two decimal points worth. (e.g. 25.43)
  //But we can't so just get 1 decimal point worth
  int tempC1dec = tempLeft*10 + (int)(tempRightToDecimal(tempRight)/1000);
  //casting to int always rounds down.

  return(tempC1dec);
  
}



//Function to get the time since starting
String getTimerVal(){
  time_t t = now();
  unsigned long secTime = second(t);
  unsigned long minTime = minute(t);
  unsigned long hourTime = hour(t);
  unsigned long timeVal = (unsigned long int)60*(unsigned long int)60*hourTime + (unsigned long int)60*minTime + secTime;
  
  String timeValStr = String(timeVal);
  return(timeValStr);
  
}

//Match the I2C address to a readable format
String getDeviceID(byte AT30TS750_I2C){
  String deviceIDval;
  if (AT30TS750_I2C == 0x4B) deviceIDval="4B";
  if (AT30TS750_I2C == 0x49) deviceIDval="49"; 
  if (AT30TS750_I2C == 0x48) deviceIDval="48";
  if (AT30TS750_I2C == 0x4F) deviceIDval="4F"; 
  return deviceIDval;
}


//Getting labeled readings--just for setup purposes
void getReadingLong(byte AT30TS750_I2C){
  errorStatus = I2c.read(AT30TS750_I2C, AT30TS750_REG_TEMP, 2, temperature);
  if (errorStatus != 0) I2CError(errorStatus);

 // Serial.print("Time: ");
 // Serial.print(second());
  //Serial.print(", ");

//  Serial.print("Temperature: ");
//  int tempLeft = temperature[0];
//  int tempRight = temperature[1];  
  //Serial.print(tempLeft);
  //Serial.print    (".");
  //Serial.println(tempRightToDecimal(tempRight));

  delay(100);  
}



//Function for the set-up to get us ready to get this experiment going.
//This is not written to the SD card
void startExperiment(){
 Serial.println("Ready?  Countdown...");
 Serial.println("5...");
 delay(1000);
 Serial.println("4...");
 delay(1000);
 Serial.println("3...");
 delay(1000);
 Serial.println("2...");
 delay(1000);
 Serial.println("1...");
 delay(1000);
 Serial.println("!!CHANGE TEMPERATURE!!");
 Serial.println();
 Serial.println("Time, Temperature");

}

//Manage the I2C errors
void I2CError(byte errorCode) {
  errorCount++;
  Serial.print   ("Error Code  = 0x");
  Serial.print   (errorCode, HEX);
  Serial.print   (" Error Count =  ");
  Serial.print   (errorCount);
  Serial.println ();
}

//Start up the sensor and the I2C communication
void init_AT30TS750(byte AT30TS750_I2C) {
  errorStatus = I2c.write  (AT30TS750_I2C, AT30TS750_REG_CONFIG, AT30TS750_REG_TEMP_RESOLUTION);
  if (errorStatus != 0) I2CError(errorStatus);
//set the resolution of the temperature sensors (can range from 9 bit to 12 bit)
}


//Get part of the temperature readout into normal decimal space
//tweak this depending on the precision situation
int tempRightToDecimal(byte byteIn){
    byte byteOne      = (byteIn & B10000000) >> 7;
    byte byteTwo      = (byteIn & B01000000) >> 6;
    byte byteThree      = (byteIn & B00100000) >> 5;
    byte byteFour      = (byteIn & B00010000) >> 4;
    
    /*
    Serial.print(byteIn);
    Serial.print("; ");
    Serial.print(byteOne);
    Serial.print(byteTwo);
    Serial.print(byteThree);
    Serial.print(byteFour);
    Serial.print("; ");
    */
    
    int outputDecimal = byteOne*5000 + byteTwo*2500 + byteThree*1250 + byteFour*625;
    //Serial.println(outputDecimal);
    return(outputDecimal);
  }


