/*
  //Try to fade from red to yellow using function and limiting the range of the input
  //Full hue space is 0 to 306 (R Y G B C R)
  //Red 300 to 360 Red 0 to 60Green 60 to 120Green 120 to 180
*/

void ledStartup() {
  // initialize the digital pin as an output.
  //  pinMode(led, OUTPUT);
  pinMode(outputRed, OUTPUT);
  pinMode(outputBlue, OUTPUT);
  pinMode(outputGreen, OUTPUT);
}
//////////////////////////////////////////////////////////////////////////////
void tempC1decToColor(int tempC1dec) 
{
  //Change the color of the LEDs
  //turn oC*10 to oF*10
  int fahrenheit1dec = celciusToFahrenheit1dec(tempC1dec);
  //normalize to get from 0 to 255 overall (scales the temp relative to the temp range
  int state = normalize(fahrenheit1dec);
  //turn the temperature state of affairs into a color (i.e. value from 0 to 360)
  //We want just the red/greens, so this function does some chopping off of color wheel sections!
  int hue = stateToColor(state);

  //  Serial print for debugging
  // Serial.println(tempLeft);
  //  Serial.println(tempRightToDecimal(tempRight));
  //  Serial.print("Temp c * 10: ");
  //  Serial.print(tempC1dec);
  //  Serial.print(", Temp F * 10: ");
  //  Serial.print(fahrenheit1dec);
  //  Serial.print(", Temp F: ");
  //  Serial.print((int)fahrenheit);
  //  Serial.print(", state: ");
  //  Serial.print(state);
//  Serial.print(", hue: ");
//  Serial.println(hue);

  //Make the color happen
  //  Serial.println(hue);
  outputData(hue);
  converter.hsvToRgb((double)hue/360.0,1,1,rgb);
    for(int i = 0;i<3;i++){
    strip.setPixelColor(i,rgb[0],rgb[0],rgb[1]);
  strip.show();                     // Refresh strip

  }

//  float temp_hue = (float) hue;
//  byte* r;
//  byte* g;
//  byte* b;
//    Serial.println("RGB stuff");
//  HSV_to_RGB(temp_hue, 1 , 1, r, g, b);
//
//  Serial.println(*r);
//  Serial.println(*g);
//   Serial.println(*b);
//   for(int i = 0;i<5;i++){
//    strip.setPixelColor(i,(uint8_t)*r,(uint8_t)*g,(uint8_t)*b);
//    strip.show();
//  }

}
////////////////////////////////////////////////////////////////////////////////
//turn the temperature state of affairs into a color
int stateToColor(int state) {
  //Red is 300-360
  //Red to yellow to green is 0 to 180
  //240 degrees in all; 1/4 of them in high red
  //so put 255 into quarters (256/4=64)
  //int casting drops decimal points (rounds down)
  //Actually I can get a lot/enough just by going from 0 to 120 (R to G)
  int hue;
  hue = map(state, 0, 255, 120, 0);

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


///////////////////////////////////////////////////////////////////////////////
//Normalize the temperature
//we go back to decimal values here instead of multiplied by 100
int normalize(int fahrenheit1dec) {
  //low and high should be globally defined
  int state = normf(fahrenheit1dec, lowReading1dec, highReading1dec);
  return (state);
}

//////////////////////////////////////////////////////////////////////////////

void outputData(int hue) {
  //turn on the lights accordingly
  setLedColorHSV(hue, 1, 1);
}

//////////////////////////////////////////////////////////////////////////////
void ledThreeColorTest() {
  uint32_t ledTimeCheck = (millis() - starttime) % 3000;
  if (ledTimeCheck > 0 && ledTimeCheck < 1000)
    setLedColor(255, 0, 0);
  if (ledTimeCheck > 1000 && ledTimeCheck < 2000)
    setLedColor(0, 255, 0);
  if (ledTimeCheck > 2000 && ledTimeCheck < 3000)
    setLedColor(0, 0, 0);

  //try to get blue...
  setLedColor(0, 255, 0);

  //I can't analogue write on pin 9????? aaaargh
  /*
    digitalWrite(9,HIGH); //blue pin
    delay(1000);
    digitalWrite(9,LOW); //blue pin
  */

  /*
    setLedColor(0, 0, 0);
    delay(100);
    setLedColor(255, 255, 255);
    delay(100);
    redGreenCircleFade();
    colorOn(1);
    redGreenFade();
    colorOn(120);
    greenRedFade();
    redYellowFade();
    yellowRedFade();
  */
}
//////////////////////////////////////////////////////////////////////////////
void redGreenCircleFade() {
  for (hue = 1; hue < 120; hue++) {
    setLedColorHSV(hue, 1, 1);
    delay(100);
  }
  for (hue = 119; hue > 0; hue--) {
    setLedColorHSV(hue, 1, 1);
    delay(100);
  }
}

//////////////////////////////////////////////////////////////////////////////
void colorOn(int hue) {
  setLedColorHSV(hue, 1, 1);
  delay(5000);
}

//////////////////////////////////////////////////////////////////////////////
void redFade() {
  for (hue = 301; hue < 360; hue++) {
    setLedColorHSV(hue, 1, 1);
    delay(100);
  }
}

//////////////////////////////////////////////////////////////////////////////
void redYellowFade() {
  for (hue = 1; hue < 60; hue++) {
    setLedColorHSV(hue, 1, 1);
    delay(100);
  }
}

//////////////////////////////////////////////////////////////////////////////
void yellowRedFade() {
  for (hue = 60; hue > 0; hue--) {
    setLedColorHSV(hue, 1, 1);
    delay(100);
  }
}

//////////////////////////////////////////////////////////////////////////////
void redGreenFade() {
  //int hue2 =0;
  for (hue = 1; hue < 180; hue++) {
    setLedColorHSV(hue, 1, 1);
    delay(100);
  }
}

//////////////////////////////////////////////////////////////////////////////
void greenRedFade() {
  //int hue2 =0;
  for (hue = 119; hue > 1; hue--) {
    setLedColorHSV(hue, 1, 1);
    delay(100);
  }
}


//////////////////////////////////////////////////////////////////////////////
void yellowGreenFade() {
  //int hue2 =0;
  for (hue = 60; hue < 180; hue++) {
    setLedColorHSV(hue, 1, 1);
    delay(100);
  }
}

//////////////////////////////////////////////////////////////////////////////
void redIntensity() {
  //just red increase in intensity (off to on)
  for (red = 0; red < 256; red++) {
    setLedColor(red, 0, 0);
    delay(10);
  }
}

//////////////////////////////////////////////////////////////////////////////
void yellowIntensity() {
  //get to yellow by increasing both green and red (off to on)
  for (yellow = 0; yellow < 256; yellow++) {
    setLedColor(yellow, yellow, 0);
    delay(10);
  }
}

//////////////////////////////////////////////////////////////////////////////
void greenIntensity() {
  //just green increase in intensity (off to on)
  for (green = 0; green < 256; green++) {
    setLedColor(0, green, 0);
    delay(10);
  }
}


//////////////////////////////////////////////////////////////////////////////
//Sets the current color for the RGB LEDs (single LED setup)
void setLedColor(int red, int green, int blue) {
  //Note that we are reducing 1/4 the intensity for the green and blue components because
  //  the red one is too dim on my LED. You may want to adjust that. <--NOTE FROM PAST CODE; WHERE DOES THIS HAPPEN?
  analogWrite(outputRed, red); //Red pin
  analogWrite(outputGreen, green); //green pin
  analogWrite(outputBlue, blue); //blue pin
  strip.show();

  //for debugging
  //showColors(red, green, blue);
}

//////////////////////////////////////////////////////////////////////////////
void setLedColorHSV(int h, double s, double v) {
  //HUE SHOULD BE FROM 0 to 360!!!  *why were we using 255?
  //this is the algorithm to convert from RGB to HSV
  double r = 0;
  double g = 0;
  double b = 0;

  //divide the hue by 60 for some reason
  //this makes more sense when hue is 0 to 360.
  double hf = h / 60.0;
  //divide the hue by 60 and round to an integer
  //this integer determines the color case space we're in
  //floor is possibly not working
  //okay, casting to int should discard the decimal points, so just do that?
  int i = (int) h / 60;
  //  double hh = floor(h/60.0);
  //  int i = (int)hh;
  //  int i=(int)floor(h/60.0);
  //here get the decimale part of the hue
  double f = h / 60.0 - i;
  double pv = v * (1 - s);
  double qv = v * (1 - s * f);
  double tv = v * (1 - s * (1 - f));

  //we choose which case to use depending on the value of i
  //I would like to confine myself to to RG space (no blue)

  switch (i)
  {
    case 0: //rojo dominante (red)
      r = v;
      g = tv;
      b = pv;
      break;
    case 1: //verde (green)
      r = qv;
      g = v;
      b = pv;
      break;
    case 2:
      r = pv;
      g = v;
      b = tv;
      break;
    case 3: //azul (blue)
      r = pv;
      g = qv;
      b = v;
      break;
    case 4:
      r = tv;
      g = pv;
      b = v;
      break;
    case 5: //rojo (red)
      r = v;
      g = pv;
      b = qv;
      break;
  }

  //set each component to a integer value between 0 and 255
  int red = constrain((int)255 * r, 0, 255);
  int green = constrain((int)255 * g, 0, 255);
  int blue = constrain((int)255 * b, 0, 255);

  //use a function to turn on each of the lights at the right amount
  setLedColor(red, green, blue);
  //try to trick this stupid thing into working
  //  delay(100);
}

//
