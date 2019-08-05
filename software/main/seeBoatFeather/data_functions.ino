/*
functions to get data from each of the sensors and put it together into a data string to send
device id, time, water, temperature, conductivity, turbidity, gpslat, gpslong (8 values) 
*/

//////////////////////////////////////////////////////////////////////////////
void sensorSetup(){
    pinMode(waterPin, INPUT);
//    pinMode(tempPin, );
  pinMode(power, OUTPUT);
  pinMode(sensor, INPUT);
  //start up the temperature sensor
  init_AT30TS750A();

}
//////////////////////////////////////////////////////////////////////////////
void init_AT30TS750A() {
 
  /* Change the resolution of the temperature measurement
       0x00 =  9 bit resolution
       0x20 = 10 bit resolution
       0x40 = 11 bit resolution
       0x60 = 12 bit resolution
  */
  Wire.begin();
  Wire.beginTransmission (AT30TS750_I2C);  
  Wire.write             (REG_CONFIG);
//  Wire.write             (0x00);            // Set Measurement Resolution Here
  Wire.write             (0x60);            // Set Measurement Resolution Here
  Wire.endTransmission   ();
}
/////////////////////////////////////////////////////////////////////////////
void measureTemp(){

  Wire.beginTransmission  (AT30TS750_I2C);
  Wire.write              (REG_TEMP);
  byte error = Wire.endTransmission    ();
//  Serial.print(error);
  
  Wire.requestFrom        (AT30TS750_I2C, 2);
  tempMSByte              = Wire.read();
  tempLSByte              = Wire.read() >> 4;

  floatTemperature        = tempMSByte + (tempLSByte * .0625);
  tempVal = floatTemperature;

  /*  FOR DEBUGGING CODE
  Serial.print    ("Raw Output Byte 01: 0x");
  Serial.println  (tempMSByte, HEX);
  Serial.print    ("Raw Output Byte 02: 0x");
  Serial.println  (tempLSByte, HEX);  
 
  Serial.print    ("Raw Output Value 01: ");
  Serial.println  (tempMSByte, DEC);
  Serial.print    ("Raw Output Value 02: ");
  Serial.println  (tempLSByte * .0625, DEC);  
 
    
  Serial.print    ("The temperature is... ");
  Serial.print    (floatTemperature, 4);
  Serial.print    ("C, ");
  */
 // Serial.print    ("The temperature is: ");
 // Serial.print    ((floatTemperature * (1.8)) + 32, 4);
 // Serial.println  ("F");

}

/////////////////////////////////////////////////////////////////////////////
void measureWater(){
  //acts like a button (high wire, low wire, reading wire)
  //they all need to be somewhat close to each other
  //if they are close and read HIGH then it's in water
  //otherwise it's in air.
    int buttonState = digitalRead(waterPin);

  // check if the sensor is in water
  // if it is, the pinState is HIGH:
  if (buttonState == HIGH) {
    waterVal = 1;
  } else {
    waterVal = 0;
  }

}

//////////////////////////////////////////////////////////////////
void measureTurbidity(){
  //Serial.print("About to get frequency avg. ");
  
  MainSensor = getFrequency(readingMain);               //returns frequency in Hz
  //Serial.println("post gottenFrequency");
  kilohertz=getFrequency(readingMain) / 1000.0;           //converts frequency to kHz 

  /*
  Serial.print("Average Frequency (Hz) = ");               
  Serial.println(MainSensor, 4);
  Serial.println("              ");
  */

  microIrradiance= kilohertz / .88;               //calculates irradiance
  milliIrradiance= microIrradiance / .001;
  
 /*
  Serial.print("Irradiance (mW/cm²) =  ");
  Serial.println(milliIrradiance, 4);
  Serial.println("____________________________________");

  delay(1000);
  */

  float slope = 0;
  float intercept = 0;
  if (boatName == "turtle") {
    slope = 0.0660060801;
    intercept = -827.591705; }
  else if (boatName == "dolphin") {
    slope = 0.07719528492;
    intercept = -432.8704854; }
  else {
      Serial.println("Name: '" + boatName + "' is not a valid boat name.");
      return; }
  
  turbVal = (kilohertz - intercept)/slope;

}

long getFrequency(int pin) {
  #define SAMPLES 500
  long freq = 0;
  for(unsigned int j=0; j<SAMPLES; j++) 
  //250000 is the maximum time we'll wait for a trigger from another pin
    freq+= 500000/pulseIn(pin, HIGH, 250000);
  return freq / SAMPLES;
}

////////////////////////////////////////////////////////////////////////
void measurepH(){
  //RX=SCL
  //TX=SDA
  
  
  if (ask_for_data){
    Wire.beginTransmission(address);
    Wire.write('r');
    Wire.endTransmission();
    ask_for_data=0;
  }
  if(millis()-pH_timer>=900){
    ask_for_data=1;
    pH_timer = millis();
    Wire.requestFrom(address,20,1);
    while (Wire.available()) {         //are there bytes to receive.
      in_char = Wire.read();           //receive a byte.
      ph_data[i] = in_char;            //load this byte into our array.
      i += 1;                          //incur the counter for the array element.
      if (in_char == 0) {              //if we see that we have been sent a null command.
        i = 0;                         //reset the counter i to 0.
        Wire.endTransmission();        //end the I2C data transmission.
        break;                         //exit the while loop.
      }
   }
   pHVal=0;
   pHVal+=ph_data[1]-'0'; //convert char array to float 
   pHVal+=(ph_data[3]-'0')*.1;
   pHVal+=(ph_data[4]-'0')*.01;
   //Serial.print("pH: ");
    //Serial.println(pHVal);
  }
 
}

//////////////////////////////////////////////////////////////////////////////
void measureConductivity(){
  tone(power,10000);
  val = analogRead(sensor);

  //analog read goes from 0-1023; our range of voltage goes from 0 to 3.3, so scale things accordingly to get a voltage value
  //But we're working from half ground, so the lowest we'll actually every read off of analogue read is half of 1023
  //map this into the full voltage range (e.g. 1023/2 should be zero volts; 1023 should go to 3.3 volts)
  float voltage = mapFloat(val, 1023/2, 1023, 0, 3.3);

  //NOTE: the conductivity code in the SeeBoat Feather code also adjust for the temperature (this impacts conductivity)

  //convert voltage to conductivity (microS)
  if (whichResistor == "1.2") {
    condVal = voltToCondRes12(voltage); } //for resistor = 1.2 kohm
  else if (whichResistor == "1.0") {
    condVal = voltToCondRes12(voltage); } //for resistor = 1.0 kohm
  else {
    Serial.println("Input: '" + (String)whichResistor + "' is not a valid resistor.");
    return; }
}

  // map() function except for floats
  float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

  // converts voltage to conductivity for resistor = 1.2 kohm
  float voltToCondRes12(float voltage) {
    if (boatName == "turtle") {
      return 16470*(voltage) - 1294; }
    else if (boatName == "dolphin") {
      return 14181*(voltage) - 739.08; }
    else {
      Serial.println("Name: '" + boatName + "' is not a valid boat name.");
      return 0; }
    //return (voltage - 0.0321)/(0.00009); //for taller probe
    //return 12878*(voltage) - 819.5; //for probe A
    //return 16053*(voltage) – 1171.2; //for probe D
}

  // converts voltage to conductivity for resistor = 1 kohm
  float voltToCondRes1(float voltage) {
    if (boatName == "turtle") {
      return 19701*(voltage) - 1189; }
    else if (boatName == "boatNameTBD") {
      return 17057*(voltage) - 687.07; }
    else {
      Serial.println("Name: '" + boatName + "' is not a valid boat name.");
      return 0; }
    //return (voltage - 0.011)/0.0001; //for taller probe
    //return 15332*(voltage) - 768.52; //for probe A
    //return 19650*(voltage) – 1194.3; //for probe D
}


//////////////////////////////////////////////////////////////////////////////
void dataAssemble(){
  //Does all the measurements and puts the data together in an array ready to be sent.
  //Do the measurements
  
/*
  theData.deviceID = 13; 
  theData.GPSlat = 18.9432; 
  theData.GPSlong = 99.7281; 
  theData.tempVal = 77.7;
*/
  int boatNameNumber = 0;
  if (boatName == "turtle") {
    boatNameNumber = 1;
  }
  else if (boatName == "dolphin") {
    boatNameNumber = 2;
  }
  else {
    Serial.println("Boat name: '" + boatName + "' is not a valid boat name.");
  }
  theData.boatName = boatNameNumber;
  theData.deviceID = deviceID; 
  theData.GPSlat = GPSlat; 
  theData.GPSlong = GPSlong; 
//  theData.GPStime = GPStime;
  theData.GPShour =  GPS.hour;
  theData.GPSmin = GPS.minute;
  theData.GPSsec = GPS.seconds;
  theData.GPSms = GPS.milliseconds;  
  theData.tempVal = tempVal;
  theData.condVal = condVal;
  theData.pHVal = pHVal;
  theData.turbVal = turbVal;
  //Put the data together
//  tempDataArray[0] = deviceID;
//  tempDataArray[1] =  GPStime;
//  tempDataArray[2] =  waterVal;
//  tempDataArray[3] =  tempVal;
//  tempDataArray[4] =  condVal;
//  tempDataArray[5] =  turbVal;
//  tempDataArray[6] = GPSlat; 
//  tempDataArray[7] =  GPSlong;

//  for (int i=0; i<8; i++){
//    Serial.println(tempDataArray[i]);
//  }
//Serial.println(theData.GPStime);
}

/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////// OLD DON'T USE
/////////////////////////////////////////////////////////////////////////////////
//Get part of the temperature readout into normal decimal space (used in another function)
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

///////////////////////////////////////////////////////////////////////////////
//Start up the temperature sensor and the I2C communication
void init_AT30TS750(byte AT30TS750_I2C) {
//NEED TO RE-DO THIS WITH I2C library!!
  /*
  errorStatus = I2c.write  (AT30TS750_I2C, AT30TS750_REG_CONFIG, AT30TS750_REG_TEMP_RESOLUTION);
//  if (errorStatus != 0) I2CError(errorStatus);
//set the resolution of the temperature sensors (can range from 9 bit to 12 bit)
*/
}

//////////////////////////////////////////////////////////////////////////////
//Get a reading from the temperature sensors
//Also returns a temperature celcius*10 value that we can use to change the color of the LED
int getTempReading(byte AT30TS750_I2C){
  //NEED TO RE-DO THIS USING WIRE LIBRARY!!!
  /*
//address, register, number of bytes; 1 originally, 2 to get more decimal points
//8 bits =  1 byte, 12 bits = 1.5 bytes
//2 is how much to get, temperature is the variable to put it in.
  errorStatus = I2c.read(AT30TS750_I2C, AT30TS750_REG_TEMP, 2, temperature);

//  if (errorStatus != 0) I2CError(errorStatus);

//Get out the person readable device ID
//note that we have 4B and 48 which look super similar but aren't!
//  String deviceIDval=getDeviceID(AT30TS750_I2C);
  String deviceIDval="77";

  //Get the temp into a real temperature value
  //first 8 are before the decimal (plus one slot for the sign); next 4 are after the decimal; final 4 are junk
  
  //temperature is a byte array
  int tempLeft = temperature[0];
  int tempRight = temperature[1];  

//  String timeValStr= getTimerVal();
  
//Put together the other parts of the measurement line--the seconds and the temperature
  //translate the other side of the decimal point to something human readable
  int tempRightDec = tempRightToDecimal(tempRight);
  String dataString= "";
  dataString+=deviceIDval;
  dataString +=", ";
//  dataString += timeValStr;
//  dataString +=", ";
  dataString+=String(tempLeft);
  dataString+=".";
  //put in the necessary zero so .0625 doesn't get written as .625 because it's an int
  if (tempRightDec<1000) {
  dataString+="0";
  }
  dataString+=String(tempRightDec);
  
  //Make this into a number we can write out
  //float breaks everything, int can go up to 32,767; unsigned int to twice that
  //it would be good for us to get two decimal points worth. (e.g. 25.43)
  //But we can't so just get 1 decimal point worth
  int tempC1dec = tempLeft*10 + (int)(tempRightToDecimal(tempRight)/1000);
  //casting to int always rounds down.

  return(tempC1dec);
  */
}

//////////////////////////////////////////////////////////////////////////////
/*
void measureTemp(){
  tempVal = getTempReading(AT30TS750_I2Cc); //make sure you put the address in here
  //note: this comes out as an integer--to get the actual number divide by 10 (I believe, for 1dec version)
}
*/

//////////////////////////////////////////////////////////////////////////////
