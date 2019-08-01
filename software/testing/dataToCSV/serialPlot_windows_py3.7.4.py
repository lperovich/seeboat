#Laura August 2019
#Python 3.7.4 on Windows 10
#Reads the Seeboat data from serial, writes it to csv, and plots it in real time

#Based on: https://makersportal.com/blog/2018/2/25/python-datalogger-reading-the-serial-output-from-arduino-to-analyze-data-using-pyserial
#possible useful resources
#added this via https://stackoverflow.com/questions/12822762/pylab-ion-in-python-2-matplotlib-1-1-1-and-updating-of-the-plot-while-the-pro


import serial
import time
import csv
import matplotlib
matplotlib.use("tkAgg")
import matplotlib.pyplot as plt
import numpy as np
from datetime import datetime

filename = "seeBoatData"
serial_port = 'COM12'
baud_rate = 9600
path = "%s_%s.csv" % (filename, str(datetime.now()).translate ({ord(c): "_" for c in "!@#$%^&*()[]{};:,./<>?\|`~-=_+"}))

ser = serial.Serial(serial_port, baud_rate)
ser.flushInput()

plot_window = 20 #This is the number of points that you're plotting at once
y_var = np.array(np.zeros([plot_window]))

#Set up the plotting space
plt.ion()
fig, ax = plt.subplots()
line, = ax.plot(y_var)

while True:
    try:
        ser_bytes = ser.readline()
        try:
            #needs to be a string to write out okay, the seperate into parts by the commas
            decoded_bytes = str(ser_bytes[0:len(ser_bytes)-2].decode("utf-8"))
            decoded_bytesList= decoded_bytes.split(",")
            #print(decoded_bytes) #if you want to see what you're getting
        except:
            print("decode fail")
            continue
        with open(path,"a") as f:
            f.writelines([decoded_bytes, ", %s , %s \n " % (datetime.now().date(), datetime.now().time())]) #write this to the file
#ADD THE DATE AND TIME

       #Make a plot, add something above to select which data you want
        y_var = np.append(y_var,float(decoded_bytesList[8]))
        y_var = y_var[1:plot_window+1]
        line.set_ydata(y_var)
        ax.relim() 
        ax.autoscale_view() 
        fig.canvas.draw() 
        fig.canvas.flush_events() 
#        plt.pause(1) #added this via website to make sure we see the plot
    except:
        print("Keyboard Interrupt")
        break
