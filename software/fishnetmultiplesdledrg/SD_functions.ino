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
    Serial.println("error opening datalog.csv");
  } 
}


