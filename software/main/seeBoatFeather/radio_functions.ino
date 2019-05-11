/*
Functions to control the sending and receiving of the messages
 */
// Feather9x_RX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (receiver)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_TX


//////////////////////////////////////////////////////////////////////////////////////

///////////////
/*In compose mode:
--keep listening for messages
--if you receive a message, it gets added to the message list
--when you switch to play mode, it sends your message
--when you switch to play mode, it stores your message locally at the top of the messages list; and your ID at the top of the ID list.
--add some kind of cancel button? to not send the message you're composing
*/

/*In receive mode:
--keep listening for messages
--if you receive a new message, store that message at the top of the message variable and move everything else down
--if you recieve a new message, restart the time counter and break from the j loop to restart the message and the ID screen playing from the beginning (message variable = 0; j=0)
*/
/////////
//Function to get the radio started in the setup

void radioStartup(){
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  delay(200);
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
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
  delay(200);
}
//////////////////////////////////////////////////////////////////////
//function to send a structured message
void radioSend(){

//from the moteino code
    //fill in the struct with new values--this happens in the data assemble part    
//    theData.nodeId = 77;
//    theData.uptime = millis();
//    theData.temp = 91.23; //it's hot!
//    Serial.print("Sending struct (");
//    Serial.print(sizeof(theData));
//    Serial.print(" bytes) ... ");
//      theData.tempVal = 22;
//      theData.deviceID=77;
//      Serial.print("Seding struct (");
//      Serial.print(sizeof(theData));
//      Serial.print(" bytes) ... ");
//RF69 version
//    if (radio.sendWithRetry(GATEWAYID, (const void*)(&theData), sizeof(theData)))
//      Serial.print(" ok!");

//      Serial.print("Sending "); Serial.println(buf);

  //THIS IS CAUSING GPS ISSUES! something in this function interfears with us getting a GPS fix. maybe a delay?
  //or it's trying to use the same serial pins or something?......baaaaah I think that should be the fona, not this one....
  Serial.println("pre send");
//Serial.print("the data pH ");
//Serial.println(theData.pHVal);
//Serial.print("the data conductivity: ");
//Serial.println(theData.condVal);
  rf95.send((uint8_t *)(&theData), sizeof(theData));
  rf95.waitPacketSent();
  Serial.println("post send");
  
//    else Serial.print(" nothing...");
//    Serial.println();
//    Blink(LED,3);

 // Serial.print("Sending "); Serial.println(buf);
//  rf95.send((uint8_t *)buf, sizeof(buf));

}



//////////////////////////////////////////////////////////////////////
//function to send a message
//put the message as an argument possibly....array situation, work on this.
void radioSendOLD(){
//      uint8_t data[] = "I'm acting like a sender now";
//      uint8_t data[] = tempMessageArray;
//      uint8_t test[] = {0, 11, 27, 3, 41, 5, 68, 7};
//      rf95.send(test, sizeof(test));
//      rf95.send((uint8_t *)test, sizeof(test));

      const uint8_t temp = 12;
//      rf95.send(temp, sizeof(temp));


//CHANGE EVERYTHING TO AN INTEGER AND THEN SEND IT!!
/*
tempDataArray[0]=1.1;
tempDataArray[1]=2.4;
tempDataArray[2]=3.0;
tempDataArray[3]=4.5;
tempDataArray[4]=6.4;
tempDataArray[5]=7.3;
tempDataArray[6]=8.5;
tempDataArray[7]=9.9;
*/
      dataToInt(1);
//      rf95.send(tempDataArrayInt, sizeof(tempDataArrayInt));    


//      rf95.send((uint8_t *)dataToSend, sizeof(dataToSend));    


      //uint8_t anotherTest[] = {10, 34, 15, 77, 102};
      //rf95.send((uint8_t *)anotherTest, sizeof(anotherTest));    

//      float testData[] ={1.0, 1.7, 2.44, 3.141}; 
//      rf95.send((uint8_t *)testData, sizeof(testData));    

//MAYBE ISSUES HERE WITH DOUBLE TO uint8_t
//      rf95.send((uint8_t *)tempDataArray, sizeof(tempDataArray));    
//      rf95.send(tempDataArray, sizeof(tempDataArray));
      rf95.waitPacketSent();
      Serial.println("Sent a message I think");
      //digitalWrite(LED, LOW);
}

//////////////////////////////////////////////////////////////////////
//Function to take our data array and turn it into whole numbers by multiplying to make our lives easier
//You need to un-do this on the other end in the receive code!!!
void dataToInt(int numDec){
  //factor that we're multiplying everything by
  //int numDec = 2;
  int multiplier = pow(10,numDec);

  for (int i=0; i<messageLength; i++){
    tempDataArrayInt[i] = (uint8_t)(tempDataArray[i]*multiplier);
  }
  
}

//////////////////////////////////////////////////////////////////////
//Another attempt at a function to send a message
//put the message as an argument possibly....array situation, work on this.
void radioSend2(){
//      uint8_t data[] = "I'm acting like a sender now";
//      uint8_t data[] = tempMessageArray;
//      uint8_t test[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
//      rf95.send(test, sizeof(test));
      //Combine the deviceID that we're sending from with the emoji message
/*      tempMessageArrayID[0] = thisDeviceID;
      for (int i=0; i<messageLength ; i++){
        tempMessageArrayID[i+1] = tempMessageArray[i];        
      }
      //Send the version that includes the ID
      rf95.send(tempMessageArrayID, sizeof(tempMessageArrayID));
      rf95.waitPacketSent();
      Serial.println("Sent a message I think");
      //digitalWrite(LED, LOW);
*/
}


//////////////////////////////////////////
//Function to listen to see if anyone is talking to us
//if we hear something, we print it to serial and we send a reply back to acknowledge it
/*
void radioListen(){
  if (rf95.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    
    if (rf95.recv(buf, &len))
    {
      //digitalWrite(LED, HIGH);
      RH_RF95::printBuffer("Received: ", buf, len);
      Serial.print("Got: ");
      //this works best for numbers
      //the first number is the device ID
      for (int i=0; i<1; i++){
        Serial.println(buf[i]); //this seems to give us normal number values
          tempReceivedDeviceID=buf[i];
      }
      //the other numbers are the message
      for (int i=1; i<len; i++){
        Serial.println(buf[i]); //this seems to give us normal number values
        tempMessageReceive[i-1]=buf[i]; //store the message here at first, then we'll put it in the overall message string
      }
      //OLD: Serial.println((char*)buf);
       Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
      //Store this message and move all the other messages down
      messageStoreReceive();
      //clear out the variables so we can receive the next message
      clearMessageVarsSend();
      delay(10);
      //reset the time (so we start playing at the beginning of the message, not halfway through) (maybe this isn't good??)
      playLoopStartTime = millis();
      j=0;


//FIND A WAY TO STORE THIS AS NORMAL NUMBERS THAT WE CAN USE LIKE BEFORE
//Right now we are getting things like: 3 6 9 C 63 63 63 63 63 63 63 63 63 63 63 
//(63=99 in hex)
      
    // Send a reply
      uint8_t data[] = "And hello back to you";
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
      Serial.println("Sent a reply");
      //digitalWrite(LED, LOW);
   */
   /*   
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
}
*/
