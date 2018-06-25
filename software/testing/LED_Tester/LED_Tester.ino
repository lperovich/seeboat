//Rod code Fall 2017
//code to verify the LED on the breakout board (the pins changed from one version to another
//we never got the blue LED to work on this breakout board (v3.0)!

int oldRed = 11;
int oldGreen = 10;
int oldBlue = 9;

int strip1 = 11;
int strip2 = 13;

int Red = 12;
int Green = 6;
int Blue = 9;

bool old = 0;
void setup() {
  // put your setup code here, to run once:
  pinMode(Red,OUTPUT);
  pinMode(Green,OUTPUT);
  pinMode(Blue,OUTPUT);
  pinMode(oldRed,OUTPUT);
  pinMode(oldGreen,OUTPUT);
  pinMode(strip1,OUTPUT);
  pinMode(strip2,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(old){
    digitalWrite(oldGreen,LOW);
    digitalWrite(oldRed,HIGH);
    delay(1000);
    digitalWrite(oldRed,LOW);
    digitalWrite(oldBlue,HIGH);
    delay(1000);
    digitalWrite(oldBlue,LOW);
    digitalWrite(oldGreen,HIGH);
    delay(1000);
  }
  else{

    digitalWrite(Red,HIGH);


    digitalWrite(Blue,HIGH);


    digitalWrite(Green,HIGH);
    digitalWrite(strip1,HIGH);
    digitalWrite(strip2,HIGH);
  }
}
