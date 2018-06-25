/*
Functions to control the management of the messages
 */


//////////////////////////////////////////////////////////////////////////////////////
//clear the soft button message after sending a message
void clearMessageVarsSend() {
  //if I press the external button into composing mode, I get rid of the last playback stuff and temp message
  //make sure to do this AFTER storing the messages
  charCounter = 0;
 //start again at the first message when I switch to display mode, not the message I've navigated to w/arrows
  messageCounter = 0;
  //Clear in the sending case
  for (int m = 0; m < messageLength; m++) {
    tempDataArray[m] = 99;
}
}
///////////////
//move the messages down a slot and store the temp message as a real one--when SENDING (really should make a better function that does it for both send and receive)
void messageStoreSend() {
  //Move the messages one down in the array
  for (int p = messageNum - 1; p >= 0; p= p-1) {
     //clear out the current array (the message has already been moved out of it)
    for (int m = 0; m < messageLength; m++) {
      dataArray[p][m] = 99;
    }
    //Move the message in front of it, into it's slot, if we're not at slot zero
    for (int m = 0; m < messageLength; m++) {
      dataArray[p][m] = dataArray[p - 1][m];
    }
  }
  //Put the temporary message into storage as message zero
  for (int m = 0; m < messageLength; m++) {
    dataArray[0][m] = tempDataArray[m];
  }

}


//////////////
//function that returns the number of real values (not 99s) in a message
int actualMessageLength(int messageCounter){
    realMessageLength=0;
     for (int s=0; s<messageLength; s++){
         arrayVal = dataArray[messageCounter][s];
        if (arrayVal != 99){
         realMessageLength = realMessageLength+1; 
        }
   }
}
///////////////
//function that verifies that there are some contents in the message (not all 99) before I send it
boolean messageCheck(){
    int tempMessageLength=0;
     for (int s=0; s<messageLength; s++){
         arrayVal = tempDataArray[s];
        if (arrayVal != 99){
         tempMessageLength = tempMessageLength+1; 
        }
     }
    if (tempMessageLength>0){
      return true;
    }
    if (tempMessageLength==0){
      return false;
    }
}

/////////////
//Function to print the contents of the message array to serial
void serialPrintMessageArray(){
    //see what's in the array
      for (int j = 0; j < messageNum; j++) {
        for (int k = 0; k< messageLength; k++){
      //get the value from each slot of the array
      arrayVal = dataArray[j][k];
      Serial.print(arrayVal);
      Serial.print(", ");
        }
      Serial.println("");
      }
}

/////////////
//Function to print the contents of the temp message array to serial
void serialPrintTempMessageArray(){
        for (int k = 0; k< messageLength; k++){
      //get the value from each slot of the array
      arrayVal = tempDataArray[k];
      Serial.print(arrayVal);
      Serial.println("");
      }
}


