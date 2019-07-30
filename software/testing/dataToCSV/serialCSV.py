#Laura 7/29/19
#Code to save the data written to the Serial port from SeeBoat and put it into a more useable CSV file

#NOTES ON USING THIS CODE
#run the code in terminal on Linux. Through the terminal, navigate to the folder it is in and run "sudo python anotherTest.py"
#Use cntr-C in terminal to stop the data collection
#The data file will be saved in the folder that the code is in. 
#Don't try to run it by double clicking, this doesn't seem to work. It also doesn't work if you don't run as super user
#Don't try to open or have open the serial port in Arduino while this is running--it will get angry and quit saving data
#You can (maybe) open the file it's writing to as it's happening, but it won't update automatically. You will see the new data if you close and re-open it.

import serial
from datetime import datetime

#datetime.datetime.now().time()
 
sensor = "SeeBoat"
serial_port = '/dev/ttyACM1'
baud_rate = 9600
path = "%s_%s.csv" % (sensor, str(datetime.now()))
ser = serial.Serial(serial_port, baud_rate)
with open(path, 'w+') as f:
    while True:
        line = ser.readline()
        f.writelines([line.strip(), ", %s , %s \n " % (datetime.now().date(), datetime.now().time())])

