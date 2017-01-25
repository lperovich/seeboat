//Functions to change the color of the LED used as output 
//See original thermal fishing bob code

/////////////////////////////////////////////////////////////////////////////
//Functions for the LED output on the thermal fishing bob

//---------------------------------------------------------------------------
//Set-up the lights
void lightSetup(){
  pinMode(outputRed,OUTPUT);
  pinMode(outputGreen,OUTPUT);
  pinMode(outputBlue,OUTPUT);
}

//---------------------------------------------------------------------------
//Function that inputs the measured value, the lowTemp value and the highTemp value to this; it outputs a state 
int normf(int x, int low, int high) {
  int y = (x - low) * 255.f / (high - low); // (temp distance from lowest temp)/(temp range)* 255 (when rearranged)==>rescaling
  //don't go above upper limit
  if(y > 255) {
    y = 255;
  }
  //don't go below lower limit
  if(y < 0) {
    y = 0;
  }
  
  //LOOK AT THIS
  //FLIP IT, since 0 and 255 are reversed for us we think?
  //y = 255-y;
  
  return y; //this is a value from 0 to 255
}

//---------------------------------------------------------------------------
//Sets the current color for the RGB LEDs (single LED setup)
void setLedColor(int red, int green, int blue) {
  //Note that we are reducing 1/4 the intensity for the green and blue components because 
  //  the red one is too dim on my LED. You may want to adjust that. <--NOTE FROM PAST CODE; WHERE DOES THIS HAPPEN?
  analogWrite(outputRed,red); //Red pin 
  analogWrite(outputGreen,green); //green pin
  analogWrite(outputBlue,blue); //blue pin 

  //for debugging
  //showColors(red, green, blue);
}



//---------------------------------------------------------------------------
//Debugging functions
void showColors(int red, int green, int blue){
  Serial.print(", red: ");
  Serial.print(red);
  Serial.print(", green: ");
  Serial.print(green);
  Serial.print(", blue: ");
  Serial.println(blue);
  //when tested, 255 is totally off and 0 is totally on
}

//More debugging: make sure the LED is behaving as expected (R, G, B; 0-255)
void ledTest(){
//Reversed from the pro mini version!!
//0 is all the way OFF; 255 is all the way ON; red dim?
  setLedColor(255,255,255);
  delay(200);
  //blue
  setLedColor(0,0,255); 
  delay(200);
  //green
  setLedColor(0,255,0);
  delay(200);
  //red
  setLedColor(255, 0,0);
  delay(200);
  }


//---------------------------------------------------------------------------

//converting rgb to hsv--basically takes a numeric color and figures out how much of each light we need to represent it right
//Note: reconsider this design wise: how many shades do we really need?
//what are the best colors? maybe red, yellow, green has more meaning--blue isn't relevant...
//HSV==hue saturation value;  http://en.wikipedia.org/wiki/HSL_and_HSV
//0-255 value, 1, 1 are current inputs

//FROM THIS: http://eduardofv.com/2011/01/15/arduino-rgb-led-hsv-color-wheel/
void setLedColorHSV(int h, double s, double v) {
  //HUE SHOULD BE FROM 0 to 360!!!  *why were we using 255?
  
  
  //this is the algorithm to convert from RGB to HSV
  double r=0; 
  double g=0; 
  double b=0;

//divide the hue by 60 for some reason
//this makes more sense when hue is 0 to 360.
  double hf=h/60.0;
//divide the hue by 60 and round to an integer
//this integer determines the color case space we're in
  int i=(int)floor(h/60.0);
  //here get the decimale part of the hue
  double f = h/60.0 - i;
  double pv = v * (1 - s);
  double qv = v * (1 - s*f);
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
  int red=constrain((int)255*r,0,255);
  int green=constrain((int)255*g,0,255);
  int blue=constrain((int)255*b,0,255);

  //use a function to turn on each of the lights at the right amount
  setLedColor(red,green,blue);
}




//---------------------------------------------------------------------------
//CULMINATION OF OUTPUT STUFF

void outputData(int hue){
  //turn on the lights accordingly
  setLedColorHSV(hue,1,1);
}


//---------------------------------------------------------------------------
//REAL CULMINATION OF OUTPUT STUFF


void tempC1decToColor(int tempC1dec){
  //Change the color of the LEDs
  //turn oC*10 to oF*10
  int fahrenheit1dec = celciusToFahrenheit1dec(tempC1dec);
 //normalize to get from 0 to 255 for each color
  int state=normalize(fahrenheit1dec);
  //turn the temperature state of affairs into a color (i.e. value from 0 to 255)
  int hue=stateToColor(state);

  //Serial print for debugging
  //Serial.println(tempLeft);
  //Serial.println(tempRightToDecimal(tempRight));  
//  Serial.print("Temp c * 10: ");
 // Serial.print(tempC1dec);
//  Serial.print(", Temp F * 10: ");
//  Serial.print(fahrenheit1dec);
//  Serial.print(", Temp F: ");
//  Serial.print((int)fahrenheit);
//  Serial.print(", state: ");
//  Serial.print(state);
 // Serial.print(", hue: ");
 // Serial.println(hue);
  
  //Make the color happen
  outputData(hue);

}




//---------------------------------------------------------------------------

//NOTES ON PWM
//(1) we need more PWM pins to do three tri-color LED outputs.  analogWrite only works on PWM pins.  Potenially use softPWM library for this.
//(2) arduino PWM ISSUES
//The PWM outputs generated on pins 5 and 6 will have higher-than-expected duty cycles. 
//This is because of interactions with the millis() and delay() functions, which share the same internal timer used to 
//generate those PWM outputs. This will be noticed mostly on low duty-cycle settings (e.g 0 - 10) and may result in a value 
//of 0 not fully turning off the output on pins 5 and 6.
//http://playground.arduino.cc/Main/PWMallPins

//






//Functions for thermister input on the thermal fishing bob
//CODE FROM LAURA'S EARLY MULTIPLE VERSION--SOME IS JUNK

//---------------------------------------------------------------------------
//convert celcius to fahrenheit
//does it in int land--two decimal points worth (i.e *100 of actual)
//int can go to 32,000
int celciusToFahrenheit1dec(int celcius1dec){
  //this is maybe more rounding than we want to int???
  int fahrenheit1dec = (celcius1dec*1.8) + 320;
  return(fahrenheit1dec);
}



//---------------------------------------------------------------------------
//Normalize the temperature 
//we go back to decimal values here instead of multiplied by 100
int normalize(int fahrenheit1dec){
 //low and high should be globally defined
  int state = normf(fahrenheit1dec, lowReading1dec, highReading1dec);
  return(state);
   }

//---------------------------------------------------------------------------
  //turn the temperature state of affairs into a color
  int stateToColor(int state){
   int hue = map(state,0,255,359,(359*0.5)); // not the whole color wheel
   return(hue);
  }


//---------------------------------------------------------------------------
//CULMULATION OF THE THERMISTOR MEASURING STUFF
//using a pointer

/*
void thermisterMeasure(float values[], int inputPin){
  
  // take N samples in a row, with a slight delay
  takeSamples(samples, inputPin);

  // average all the samples out
  float average= getAverage(samples);
  
 //turn the average into a resistence 
   float aveResistence = aveToResistence(average);

  //turn the resitence into a degrees celcius
  float celcius=aveToCelcius(aveResistence);

  //turn oC to oF
  float fahrenheit = celciusToFahrenheit(celcius);
 
 //normalize frequency...get these input values based on the thermister(?)
  float state=normalize(fahrenheit);
  
  //turn the temperature state of affairs into a color (i.e. value from 0 to 255)
  int hue=stateToColor(state);

  //this goes into the function we use to turn on the lights and write out the state
  values[0]= average;
  values[1]= aveResistence;
  values[2]= celcius;
  values[3]= fahrenheit;
  values[4]= state;
  values[5]= hue;
}  
*/


//---------------------------------------------------------------------------
//Function to write out data to tell us what's going on
/*
void showState(){
      // 0 average; 1 aveResistence; 2 celcius; 3 fahrenheit; 4 state; 5 hue;
    Serial.print("average: ");
  Serial.print(values[0]);
    Serial.print("; aveResistence: ");
  Serial.print(values[1]);
    Serial.print("; celcius: ");
  Serial.print(values[2]);
    Serial.print("; fahrenheit: ");
  Serial.print(values[3]);
    Serial.print("; state: ");
  Serial.print(values[4]);
    Serial.print("; hue: ");
  Serial.println(values[5]);
}
*/
//
