// Feather9x_RX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (receiver)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_TX

//Attempt by Laura to combine the Tx and Rx functionality
//The radio should have a button. Pressing that button shifts it from one mode to another

//RADIO VARIABLES
#include <SPI.h>
#include <RH_RF95.h>
/* for feather m0 */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
// Blinky on receipt
#define LED 13

//MODE BUTTON VARIABLES
const int buttonPin = 12;    // the number of the pushbutton pin
int buttonCount = 0;  //the number of times the button presses or un-presses
float buttonPresses = 0; //the actual number of times the button has been pressed
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

//for Tx
int16_t packetnum = 0;  // packet counter, we increment per xmission

//variable for telling us what mode we're in
//0 is Rx; 1 is Tx
int transMode = 0;

/////////////////////////////////////////////////////////////
void setup() 
{
  pinMode(buttonPin, INPUT);
  pinMode(LED, OUTPUT);     
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  //take this serial line out if you're not necessarily connected to the computer!
  //while (!Serial);
  Serial.begin(9600);
  delay(100);

  Serial.println("Feather LoRa Tx-RX Test by Laura!");
  
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}


////////////////////////////////////////////////////////////////////////////////////

void loop()
{
//See what mode we're in and act accordingly

//WATCH THE BUTTON
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);
  // check to see if you just pressed the button (i.e. the input went from LOW to HIGH),  
  //and you've waited long enough since the last press to ignore any noise:
  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce delay, so take it as the actual current state:
    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;
        //increment the count
        buttonCount = buttonCount+1;
      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        ledState = !ledState;
      }
    }
  }
  // set the LED:
  digitalWrite(LED, ledState);
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;

transMode = buttonCount/2 %2;
Serial.print("TRANS MODE: ");
Serial.println(transMode);
Serial.print("BUTTON COUNT: ");
Serial.println(buttonCount);


//RECEIVING
if (transMode==0){
  if (rf95.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    
    if (rf95.recv(buf, &len))
    {
      digitalWrite(LED, HIGH);
      RH_RF95::printBuffer("Received: ", buf, len);
      Serial.print("Got: ");
      Serial.println((char*)buf);
       Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
      delay(10);
      // Send a reply
      uint8_t data[] = "And hello back to you";
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
      Serial.println("Sent a reply");
      digitalWrite(LED, LOW);
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
}
//When I want to send something
// Send a reply
if (transMode==1){
      uint8_t data[] = "I'm acting like a sender now";
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
      Serial.println("Sent a message");
      digitalWrite(LED, LOW);
}

}


///////////////////////////////////////////////////////////////





