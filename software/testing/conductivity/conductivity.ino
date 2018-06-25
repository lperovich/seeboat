//Works with the Adafruit feather MO
//code by Rod Fall 2017
//Wiring should be the same as used on the SeeBoat (see the print out of the conductivity board with labels)


int power = 11;              //50% square wave (yellow wire)¯Â
int sensor = A1;             //final voltage (purple wire)
float val = 0.0;


//also use 3V pin (red wire)
//GND pin (grey wire)


///////////////////////////////////// SET-UP
void setup() {
  Serial.begin(9600);
  pinMode(power, OUTPUT);
  pinMode(sensor, INPUT);
}

///////////////////////////////////// GET VALUES
void loop() { 
  tone(power,10000);
  val = analogRead(sensor);

  //analog read goes from 0-1023; our range of voltage goes from 0 to 3.3, so scale things accordingly to get a voltage value
  //But we're working from half ground, so the lowest we'll actually every read off of analogue read is half of 1023
  //map this into the full voltage range (e.g. 1023/2 should be zero volts; 1023 should go to 3.3 volts)
  float voltage = map(val, 1023/2, 1023, 0, 3.3);

  //NOTE: the conductivity code in the SeeBoat Feather code also adjust for the temperature (this impacts conductivity)
 
  Serial.print("Voltage: "); 
  Serial.println(voltage);



}

