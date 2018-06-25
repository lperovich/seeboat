//Code by Sophia Summer 2017
//pinout as in the pdf diagram draw by Sophia
//For the adafruit feather board; a version of this code modified by Rod is the the SeaBoatFeather code

//LP note: there may be some issues in this code--why do we do a measurement for MainSensor and then again for KiloHertz???


////////////////////////////////////// SET UP
int readingMain = 10;
//int power = 11;
//int ground = 9;

float MainSensor;
float microIrradiance;
float irradiance;
float kilohertz;
float milliIrradiance;

void setup() {
  Serial.begin(9600);                                  // initiate serial communication
//  pinMode(power, OUTPUT);
  pinMode(readingMain, INPUT);
//  pinMode(ground, OUTPUT);
  while(!Serial){;}         
}

////////////////////////////////////// LOOP

void loop() {


//  digitalWrite(power, HIGH);
//                      
//  digitalWrite(ground, LOW);                          // LOW turns IR on
// 
  Serial.print("About to get frequency avg. ");
  
  MainSensor = getFrequency(readingMain);               //returns frequency in Hz
  Serial.println("post gottenFrequency");
  kilohertz=getFrequency(readingMain) / 1000.0;           //converts frequency to kHz 

  Serial.print("Average Frequency (Hz) = ");               
  Serial.println(MainSensor, 4);
  Serial.println("              ");


  //Serial.print("Average Frequency (kHz) = ");               
  //Serial.println(kilohertz, 4);
  
  microIrradiance= kilohertz / .88;               //calculates irradiance
  milliIrradiance= microIrradiance / .001;
  
 
  Serial.print("Irradiance (mW/cm²) =  ");
  Serial.println(milliIrradiance, 4);
  Serial.println("____________________________________");

  

}

////////////////////////////////////// FUNCTIONS

long getFrequency(int pin) {
  #define SAMPLES 500
//  #define SAMPLES 100
  long freq = 0;
  for(unsigned int j=0; j<SAMPLES; j++) 
    freq+= 500000/pulseIn(pin, HIGH, 250000);
  return freq / SAMPLES;
}

