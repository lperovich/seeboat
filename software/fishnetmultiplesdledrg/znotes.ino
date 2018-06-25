// NOTES
//assorted info that was useful in putting together the code




/*
Trying to write to a new filename each restart/code run or something
someone else attempts to do this: http://forum.arduino.cc/index.php?topic=143820.0
Dealing with character arrays: http://forum.arduino.cc/index.php?topic=41271.0
random number generation: https://www.arduino.cc/en/Reference/Random

*/


/*
In order for the Master to select and access the AT30TS750A, the Master must first initiate a Start condition. Following
the Start condition, the Master must output the device address byte. The device address byte consists of the 7-bit device
address plus a Read/Write (R/W) control bit, which indicates whether the Master will be performing a Read or a Write to
the AT30TS750A. If the R/W control bit is a Logic 1, then the Master will be reading data from the AT30TS750A.
Alternatively, if the R/W control bit is a Logic 0, then the Master will be writing data to the AT30TS750A.
*/


/*
//const byte   AT30TS750_REG_TEMP_RESOLUTION = 0x60; //0x00 for 9 bit; 0x60 for 12-bit
//0x00 should be giving us to 0.5 degree
//[[0 is write/ 1 is read]]
//conversion resolution (bit 14:13) (R1:R0)(NVR1:NVR0)
//00 = 9 bits default (0.5oC, 25 ms)
//01 = 10 bits (0.25oC, 50 ms)
//10 = 11 bits (0.125oC, 100 ms)
//11 = 12 bits (0.0625oC, 200 ms)
*/

/*
//fix I2C errors via this?  (scroll partway down--change uint8_t to size_t?)
//https://forum.pjrc.com/threads/21680-New-I2C-library-for-Teensy3/page6
*/

/*
//getting the erro 0x1 which means:
//Timeout: waiting for confirmation of Start Condition
//i.e. it can't talk to the board
*/

/*
//Notes on I2C.write:
// I2c.write(address, registerAddress, data) - initate an I2C write operation, single data byte.
//Typically used to send a single byte of data to a register address
//Parameters:   (uint8_t)address:  7 bit I2C slave address
//(uint8_t)registerAddress: Address of the register as per the datasheet
//(uint8_t)data: Single byte of data
*/

