

SeeBoatRx goes on the feather that's attached to the computer and receiving the data.

SeeBoatFeather goes on the feather that's on the boat. It needs a breakout board with sensor ports plugged into it. In theory it can also have a GPS featherwing plugged in and keep track of GPS location, but in practice this doesn't seem to work (it never gets a fix).

seeBoatGPSdataSD is a workaround for this GPS issue. It can go on an adafruit feather M0 data logging with SD card board that is plugged into an adafruit GPS with featherwing and an extended antenna and can be added to the SeeBoat to seperately log the position of the boat as it is collecting the data. This GPS information saved on the SD card can later be merged with the sensor data saved as a csv on the computer to determine when the boat was at a particular location and create a digital map of the path of the boat and the data. 

The dataToCSV folder contains python sketches for a variety of computers/versions of python. These can be run while SeeBoat is operating and the SeeBoatRx board is connected to the computer. They will display the data coming in as real time plots and at the same time they will save the received data as a csv on the computer. Always run one of these if you want to keep the data. The data is also time stamped. 

The archive folder contains code no longer in use.

Up one level, the testing folder contains sketches that test each part of the system, including each sensor, to make debugging each part individually easier and allow for greater flexibility in putting the systems together. 
