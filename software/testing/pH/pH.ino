//By Rod Fall 2017
//Is currently on the feather with the Atlas Scientific probe for the SeaBoat application
//goes with the wiring outlined in Rod's pH probe diagram


#include <Wire.h>
#define address 99

char ph_data[20];                //we make a 20 byte character array to hold incoming data from the pH circuit.
byte in_char = 0; 
byte i = 0; 
uint32_t pH_timer = millis();
bool ask_for_data = 1;
float pHVal;

void setup() {
  Serial.begin(9600);
  Wire.begin();
}

void loop() {
  pHVal=0;
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
   //Get it from a char array into a float
   pHVal+=ph_data[1]-'0'; //convert char array to float 
   pHVal+=(ph_data[3]-'0')*.1;
   pHVal+=(ph_data[4]-'0')*.01;
  }
  Serial.println("hello");
  Serial.print(pHVal);
}
