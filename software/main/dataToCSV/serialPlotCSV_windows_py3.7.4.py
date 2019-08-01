#Laura August 2019
#Talia's version--Python 3.7.4 on Windows 10
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
#There are four different sensors we're plotting data for
y_varC = np.array(np.zeros([plot_window])) #conductivity
y_varN = np.array(np.zeros([plot_window])) #turbitidy
y_varT = np.array(np.zeros([plot_window])) #temperature
y_varP = np.array(np.zeros([plot_window])) #pH

#Set up the plotting space
plt.ion()
fig, ax = plt.subplots(nrows=2, ncols=2) #this should be four subplots

lineC, = ax[0,0].plot(y_varC)
lineT, = ax[0,1].plot(y_varT)
lineN, = ax[1,0].plot(y_varN)
lineP, = ax[1,1].plot(y_varP)

ax[0,0].set_title("Conductivity")   
ax[0,1].set_title("Temperature")   
ax[1,0].set_title("Turbidity")   
ax[1,1].set_title("pH")   

# Define a function to clean up the plot axes
def plotfix( plotname ):
   plotname.relim() 
   plotname.axes.get_xaxis().set_visible(False) #get rid of x-axis labels
   plotname.autoscale_view() 
   return;

#Put in the header for the csv
with open(path, 'a') as f:
    header = ["deviceID,", "GPSlat,", "GPSlong,", "GPShour,", "GPSmin,", "GPSsec,", "GPSms,", "tempC,", "conduS,", "pH,","turbNTU,","date,", "time\n"]
    f.writelines(header)


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
            f.writelines([decoded_bytes, ", %s , %s \n " % (datetime.now().date(), datetime.now().time())]) #write this to the file, adding the date and time

       #Make a plot, add something above to select which data you want
        #Make a plot, including subplots for all sensors. Indices are how the come from arduino
        y_varC = np.append(y_varC,float(decoded_bytesList[8]))
        y_varN = np.append(y_varN,float(decoded_bytesList[10]))
        y_varT = np.append(y_varT,float(decoded_bytesList[7]))
        y_varP = np.append(y_varP,float(decoded_bytesList[9]))

        y_varC = y_varC[1:plot_window+1]
        y_varN = y_varN[1:plot_window+1]
        y_varT = y_varT[1:plot_window+1]
        y_varP = y_varP[1:plot_window+1]

        lineC.set_ydata(y_varC)
        lineN.set_ydata(y_varN)
        lineT.set_ydata(y_varT)
        lineP.set_ydata(y_varP)

        plotfix(ax[0,0])
        plotfix(ax[0,1])
        plotfix(ax[1,0])
        plotfix(ax[1,1])

        fig.canvas.draw() 
        fig.canvas.flush_events() 
#        plt.pause(1) #added this via website to make sure we see the plot
    except:
        print("Keyboard Interrupt")
        break
