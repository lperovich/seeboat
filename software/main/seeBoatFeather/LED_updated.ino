/*
 * Updated LED functions for strip of LEDs (last one was just for a single LED).
 * !!!!!!!!!!!!!For some reason, this is our order of rgb: strip.setPixelColor(index, green, red, blue);
 */

void ledStartup() {
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    for(int i = 1;i<NUMPIXELS;i++){
      strip.setPixelColor(i,0xFFFFFF); // goes through and resets all LEDs first to white
    strip.show();}
}

////////////////////////////////////////////////////////////////////////////////
// depending on string input, determines which data the LEDs should be demonstrating
void dataToColor(String dataInput) 
{
  int dataValue = 0;
  int low = 0;
  int high = 0;
  
  //Adjust for each data type
  if (dataInput == "temperature") {
      //turn oC*10 to oF*10
      dataValue = celciusToFahrenheit1dec(tempVal*10);
      low = lowReading1dec;
      high = highReading1dec; }
  else if (dataInput == "conductivity") {
      dataValue = condVal; 
      low = lowReadingCond;
      high = highReadingCond; }
  else if (dataInput == "turbidity") {
      dataValue = turbVal; 
      low = lowReadingTurb;
      high = highReadingTurb; }
  else if (dataInput == "pH") {
      dataValue = pHVal*100; //times 100 to get more decimals
      low = lowReadingPH;
      high = highReadingPH; }
  else {
    Serial.println("Input: " + (String)dataInput + " is not a valid input.");
    return; }

  //normalize to get from 0 to 255 overall (scales the data relative to the data range)
  int state = normalize(dataValue, low, high);
  
  //turn the data state of affairs into a color (i.e. value from 0 to 21845)
  //We want just the red/greens, so this function does some chopping off of color wheel sections!
  hue = stateToColor(state);

  //Make the color happen
  outputData(hue);
}

////////////////////////////////////////////////////////////////////////////////

//turn the temperature state of affairs into a color
int stateToColor(int state) {
  //int casting drops decimal points (rounds down)
  //Goes from 0 to 21845 (R to G)
  hue = map(state, 0, 255, 0, 21845);

  /*  int hueSector = (int)state/64;
    if (hueSector == 3){
     hue = map(state,0,255,359,300);
    }
    else {
      //reverse this--cold is green, warm is red
     hue = map(state,0,255,179,0);
    }
  */
  return (hue);
}

////////////////////////////////////////////////////////////////////////////////

//convert celcius to fahrenheit
//does it in int land--two decimal points worth (i.e *100 of actual)
//int can go to 32,000
int celciusToFahrenheit1dec(int celcius1dec) {
  //this is maybe more rounding than we want to int???
  int fahrenheit1dec = (celcius1dec * 1.8) + 320;
  return (fahrenheit1dec);
}

///////////////////////////////////////////////////////////////////////////////

//Normalize the temperature
//we go back to decimal values here instead of multiplied by 100
int normalize(int dataValue, int low, int high) {
  //low and high should be globally defined
  int state = normf(dataValue, low, high);
  return (state);
}

///////////////////////////////////////////////////////////////////////////////

void outputData(int hue) {
  //turn on the lights accordingly
  uint32_t rgbcolor = strip.ColorHSV(hue);
  strip.fill(rgbcolor);
  strip.show();
}

///////////////////////////////////////////////////////////////////////////////

//Function that inputs the measured value, the lowTemp value and the highTemp value to this; it outputs a state
int normf(int x, int low, int high) {
  int y = (x - low) * 255.f / (high - low); // (temp distance from lowest temp)/(temp range)* 255 (when rearranged)==>rescaling
  //don't go above upper limit
  if (y > 255) {
    y = 255;
  }
  //don't go below lower limit
  if (y < 0) {
    y = 0;
  }

  //LOOK AT THIS
  //FLIP IT, since 0 and 255 are reversed for us we think?
  //y = 255-y;

  return y; //this is a value from 0 to 255
}
