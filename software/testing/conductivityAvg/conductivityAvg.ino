//Works with the Adafruit feather MO
//code by Rod Fall 2017
//Wiring should be the same as used on the SeeBoat (see the print out of the conductivity board with labels)


int power = 5;              //50% square wave (yellow wire) connected to pin 5 on the feather
int sensor = A1;             //final voltage (purple wire)
float val = 0.0;


//also use 3V pin (red wire)
//GND pin (grey wire)


///////////////////////////////////// SET-UP
void setup() {
  Serial.begin(9600);
  pinMode(power, OUTPUT);
  pinMode(sensor, INPUT);
   //we can do 12 bit on the feather to get better data
  analogReadResolution(12);
}

///////////////////////////////////// GET VALUES
void loop() { 
  //put in the frequency here! definitely works with 10,000
  tone(power,10000);
  val = avg(sensor);

  //analog read goes from 0-1023; our range of voltage goes from 0 to 3.3, so scale things accordingly to get a voltage value
  //But we're working from half ground, so the lowest we'll actually every read off of analogue read is half of 1023
  //map this into the full voltage range (e.g. 1023/2 should be zero volts; 1023 should go to 3.3 volts)
  //10 bit
//  float voltage = mapFloat(val, 1023/2, 1023, 0, 3.3);
  //12 bit
  float voltage = mapFloat(val, 4095/2, 4095, 0, 3.3);

  //NOTE: the conductivity code in the SeeBoat Feather code also adjust for the temperature (this impacts conductivity)

  //convert voltage to conductivity (microS)
  float conductivity = voltToCondRes12(voltage); //for resistor = 1.2 kohm
  //float conductivity = voltToCondRes12(voltage); //for resistor = 1.0 kohm

  Serial.println(voltage,4);
  delay(250);

}

  //averages data
  long avg(int pin) {
  #define SAMPLES 10
//  #define SAMPLES 100
  float val = 0;
  for(unsigned int j=0; j<SAMPLES; j++) 
    val = val + analogRead(sensor);
  return val / SAMPLES;
}

  // map() function except for floats
  float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

  // converts voltage to conductivity for resistor = 1.2 kohm
  float voltToCondRes12(float voltage) {
    return (voltage)*12878 - 819.5; //for A
    //return (voltage - 0.0321)/(0.00009); //for smaller probe
    //return (voltage)*(17698) - 63.105; //for wider probe
}

  // converts voltage to conductivity for resistor = 1 kohm
  float voltToCondRes1(float voltage) {
    return (voltage)*15332 - 768.52; //for A
    //return (voltage - 0.011)/0.0001; //for smaller probe
    //return (voltage)*(21077) + 143.87; //for wider probe
}
