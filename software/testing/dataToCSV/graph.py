import serial
import time
import csv
import matplotlib
matplotlib.use("tkAgg")
import matplotlib.pyplot as plt
import numpy as np
from datetime import datetime

filename = "testData"
serial_port = '/dev/ttyACM1'
baud_rate = 9600
path = "%s_%s.csv" % (filename, str(datetime.now()))

ser = serial.Serial(serial_port, baud_rate)
ser.flushInput()

plot_window = 20 #This is the number of points that you're plotting at once
y_var = np.array(np.zeros([plot_window]))
print(y_var)

plt.ion()
#plt.show() #added this via https://stackoverflow.com/questions/12822762/pylab-ion-in-python-2-matplotlib-1-1-1-and-updating-of-the-plot-while-the-pro
fig, ax = plt.subplots()
line, = ax.plot(y_var)
print("hey")


while True:
    try:
        ser_bytes = ser.readline()
        try:
#            print(ser_bytes[0:len(ser_bytes)])
#            decoded_bytes = ser_bytes[0:len(ser_bytes)-2].decode("utf-8")
            decoded_bytes = str(ser_bytes[0:len(ser_bytes)-2].decode("utf-8"))
            decoded_bytesList= decoded_bytes.split(",")
            print(decoded_bytesList[0]) #access the first element in the list created by splitting on the commas...this seems to understand that it becomes a number?
#Maybe this is a string that I then need to divide up along the commas
#            decoded_bytes = float(ser_bytes[0:len(ser_bytes)-2].decode("utf-8"))
            print(decoded_bytes)
        except:
            print("decode fail")
            continue
        with open(path,"a") as f:
            f.writelines([decoded_bytes])
#            writer = csv.writer(f,delimiter=",")
#            writer.writerow([decoded_bytes])
#            writer.writerow([time.time(),decoded_bytes])
            print("file write")

#Try plotting just something--conductivity moves the most with no sensor
        y_var = np.append(y_var,decoded_bytesList[8])
        print(y_var)
        y_var = y_var[1:plot_window+1]
        print(y_var)
        line.set_ydata(y_var)
        plt([0,1,2,3,4,5], [3,4,5,6,7,8]).show()
 #       ax.relim() #This line causes an unexpected keyboard interrupt! why!?
        ax.autoscale_view() #this line is okay and makes the plot come up
  #      fig.canvas.draw() #This line is not okay
        fig.canvas.flush_events() #this line is okay
        plt.pause(1) #added this via the same website
        print("once through loop")
    except:
        print("Keyboard Interrupt")
#        continue
        break
