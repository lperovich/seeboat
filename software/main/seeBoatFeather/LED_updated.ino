/*
 * Updated LED functions for strip of LEDs.
 * For some reason, this is our order of rgb: strip.setPixelColor(index, green, red, blue);
 */

void ledStartup() {
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    for(int i = 1;i<NUMPIXELS;i++){
      strip.setPixelColor(i,0xFFFFFF);
    strip.show();}
}

////////////////////////////////////////////////////////////////////////////////

void tempC1decToColor(int tempC1dec) 
{
  //Change the color of the LEDs
  //turn oC*10 to oF*10
  int fahrenheit1dec = celciusToFahrenheit1dec(tempC1dec);
  
  //normalize to get from 0 to 255 overall (scales the temp relative to the temp range
  int state = normalize(fahrenheit1dec);
  
  //turn the temperature state of affairs into a color (i.e. value from 0 to 360)
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
int normalize(int fahrenheit1dec) {
  //low and high should be globally defined
  int state = normf(fahrenheit1dec, lowReading1dec, highReading1dec);
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
