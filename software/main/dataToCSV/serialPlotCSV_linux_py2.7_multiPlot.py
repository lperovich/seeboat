#Laura August 2019
#Python 2.7.15+ on Ubuntu 18.04
#Reads the Seeboat data from serial, writes it to csv, and plots it in real time

#Based on: https://makersportal.com/blog/2018/2/25/python-datalogger-reading-the-serial-output-from-arduino-to-analyze-data-using-pyserial
#possible useful resources
#added this via https://stackoverflow.com/questions/12822762/pylab-ion-in-python-2-matplotlib-1-1-1-and-updating-of-the-plot-while-the-pro

#for coloring lines by column variable
#seaborn https://stackoverflow.com/questions/14885895/color-by-column-values-in-matplotlib
#or ggplot http://ggplot.yhathq.com/

import serial
import time
import csv
import matplotlib
import seaborn as sns
#import ggplot
matplotlib.use("tkAgg")
import matplotlib.pyplot as plt
import numpy as np
from datetime import datetime
#from pandas import DataFrame
import pandas as pd

sns.set(style="ticks")


#take off the labels on the x axis
#do many plots

filename = "seeBoatData"
serial_port = '/dev/ttyACM1'
baud_rate = 9600
path = "%s_%s.csv" % (filename, str(datetime.now()))
nboats = 2 # the number of SeeBoats that you have attached to this radio

#say what colors the lines will be. Maximum of four boats
lineColors=np.array(["red", "blue", "green", "pink"])

ser = serial.Serial(serial_port, baud_rate)
ser.flushInput()

plot_window = 50 #This is the number of points that you're plotting at once

#-----
# Load the example dataset for Anscombe's quartet
#df = sns.load_dataset("anscombe")

# Show the results of a linear regression within each dataset
#sns.lmplot(x="x", y="y", col="dataset", hue="dataset", data=df,
#           col_wrap=2, ci=None, palette="muted", height=4,
#           scatter_kws={"s": 50, "alpha": 1})

#plt.show()
#--


#This is an empty data frame we're creating
#Date, time will be in the csv but not in the pandas database 
df=pd.DataFrame(columns=["boatName", "deviceID", "GPSlat", "GPSlong", "GPShour", "GPSmin", "GPSsec", "GPSms", "tempC", "conduS", "pH","turbNTU", "dataPt"])
#df=DataFrame(columns=["boatName", "deviceID", "GPSlat", "GPSlong", "GPShour", "GPSmin", "GPSsec", "GPSms", "tempC", "conduS", "pH","turbNTU","date", "time"])

#Give it some initial values so it doesn't get angry
#df["boatName"]="cow"
#df["conduS"]=0
#df["tempC"]=0
#df["turbNTU"]=0
#df["pH"]=0
#df["dataPt"]=range(1,51)

print(df)

#Turn interactive mode on!!
plt.ion()



#Maybe I skip this and work from the dataframe
#There are four different sensors we're plotting data for
#there are nboats number of boats; each boat will have one line on the graph
#These need to be 2-D arrays for the multiple boats case
#first indice is the boat, second is the data point within that boat's data for that sensor 
#y_varC = np.tile(np.array(np.zeros([plot_window])), (nboats, 1)) #conductivity
#y_varN = np.tile(np.array(np.zeros([plot_window])), (nboats, 1)) #turbitidy
#y_varT = np.tile(np.array(np.zeros([plot_window])), (nboats, 1)) #temperature
#y_varP = np.tile(np.array(np.zeros([plot_window])), (nboats, 1)) #pH


#Set up the plotting space
#plt.ion()
#fig, ax = plt.subplots(nrows=2, ncols=2) #this should be four subplots
#line, = ax.plot(y_var)

#Maps the variable names within the column to the colors we want them to map to
#gcolors={'dolphin': '#00B0F6', 'fish': '#E58700', 'shark': '#E76BF3', 'turtle': '#E76BF3', 'cow': '#FF5500'}

#plot from the dataframe, choosing the correct column for the variable and color coding the points by the boat variable. hopefully this connects the right ones too....
#lineC, = ax[0,0].plot(df["conduS"], color=[gcolors[group] for group in df["boatName"]])
#lineT, = ax[0,1].plot(df["tempC"], color=[gcolors[group] for group in df["boatName"]])
#lineN, = ax[1,0].plot(df["turbNTU"], color=[gcolors[group] for group in df["boatName"]])
#lineP, = ax[1,1].plot(df["pH"], color=[gcolors[group] for group in df["boatName"]])
#lineT, = ax[0,1].plot(y_varT)


#Put titles on the graphs
#ax[0,0].set_title("Conductivity")   
#ax[0,1].set_title("Temperature")   
#ax[1,0].set_title("Turbidity")   
#ax[1,1].set_title("pH")   

# Define a function to rescale and clean up the plot axes
def plotfix( plotname ):
   plotname.relim() 
   plotname.axes.get_xaxis().set_visible(False) #get rid of x-axis labels
   plotname.autoscale_view() 
   return;

#Put in the header for the csv
with open(path, 'a') as f:
    header = ["boatName,", "deviceID,", "GPSlat,", "GPSlong,", "GPShour,", "GPSmin,", "GPSsec,", "GPSms,", "tempC,", "conduS,", "pH,","turbNTU,","date,", "time\n"]
    f.writelines(header)

while True:
    try:
	#Save the data to a csv
        ser_bytes = ser.readline()
        try:
            #needs to be a string to write out okay, the seperate into parts by the commas
            decoded_bytes = str(ser_bytes[0:len(ser_bytes)-2].decode("utf-8"))
            decoded_bytesList= decoded_bytes.split(",")
#            print(decoded_bytesList) #if you want to see what you're getting

            #Write in the data one element at a time...there must be a faster way than this
            newIndex = len(df.index)+1
            df.loc[newIndex, 'boatName'] = decoded_bytesList[0]
            df.loc[newIndex, 'deviceID'] = decoded_bytesList[1]
            df.loc[newIndex, 'GPSlat'] = decoded_bytesList[2]
            df.loc[newIndex, 'GPSlong'] = decoded_bytesList[3]
            df.loc[newIndex, 'GPShour'] = decoded_bytesList[4]
            df.loc[newIndex, 'GPSmin'] = decoded_bytesList[5]
            df.loc[newIndex, 'GPSsec'] = decoded_bytesList[6]
            df.loc[newIndex, 'GPSms'] = decoded_bytesList[7]
            df.loc[newIndex, 'tempC'] = decoded_bytesList[8]
            df.loc[newIndex, 'conduS'] = decoded_bytesList[9]
            df.loc[newIndex, 'pH'] = decoded_bytesList[10]
            df.loc[newIndex, 'turbNTU'] = decoded_bytesList[11]
            print("hi")
#            print(df)
   

        except:
            print("decode fail")
            continue

        with open(path,"a") as f:
#            f.writelines([decoded_bytes]) #write this to the file
            f.writelines([decoded_bytes, ", %s , %s \n " % (datetime.now().date(), datetime.now().time())]) #write this to the file

        try:        
            #Make a wide version that's easier to plot
            #rename some variables so it's easier to work with
            df1 = df.rename(columns={'tempC':'S1', 'conduS':'S2', 'pH':'S3', 'turbNTU':'S4'}).reset_index()
            df_long=pd.wide_to_long(df1, stubnames=['S'], i='index', j='id').reset_index()
            df_long = df_long.replace({'id':{1:"tempC", 2:"conduS", 3:"pH", 4:"turbNTU"}})

            #Plot it
            #Cut back on the rows here later
            print(df_long)
#            ax=sns.relplot(x="dataPt", y="S", kind='line', data=df_long) 
            print(df_long.dtypes)
#
#            print(df_long.astype({"dataPt": int}))

            #FIX THE DATA TYPES. We want normal variables, not objects
#            cols = 'S'
#            df_long[cols] = df_long[cols].apply(pd.to_numeric, errors='coerce')
#            df_long['boatName'] = df_long['boatName'].apply(pd.to_string, errors='coerce')
#            df_long = df_long.infer_objects() #This fixes the dataPt column
#            df_long = df_long.astype({"dataPt": int, "S": float})
#            print(df_long.dtypes)

#PLOTTING STUFF

#            ax=sns.relplot(x="dataPt", y="dataPt", data=df_long) 
#            tips = sns.load_dataset("tips")
#            ax=sns.relplot(x="total_bill", y="tip", hue="day", data=tips)
            #ax=sns.relplot(x=[1,2,3,4,5], y=[0,1,2,3,4], kind='line') 
          #  ax=sns.relplot(x="dataPt", y="S", hue="boatName", kind='line', col='id', data=df_long, facet_kws={'sharey': False, 'sharex': True}) 

            #Gets rid of the xaxis labels
#            ax.set(xticklabels=[])
            print("d")


#            sns.lmplot(x="dataPt", y="tempC", col="boatName", hue="boatName", data=df)
#            sns.lmplot(x="dataPt", y="tempC", data=df) #so it doesn't like to plot tempC

#            plt.show()
##use draw instead of show--show is not interactive!
#            ax.draw()
#            plt.draw()
#            plt.pause(2) #Have to pause or you won't see it
#            plt.canvas.flush_events() 

#            ax.canvas.draw()
#            ax.canvas.flush_events()
#            plt.canvas.draw() 
#            plt.canvas.flush_events() 

        except:
            print("plot fail")
            continue

        #Seperate out the datalines for each boat-sensor combination
	#Four plots; nboat lines per plot


        #Make a plot, including subplots for all sensors. Indices are how the come from arduino
        #y_varC = np.append(y_varC,float(decoded_bytesList[9]))
        #y_varN = np.append(y_varN,float(decoded_bytesList[11]))
        #y_varT = np.append(y_varT,float(decoded_bytesList[8]))
        #y_varP = np.append(y_varP,float(decoded_bytesList[10]))

        #y_varC = y_varC[1:plot_window+1]
        #y_varN = y_varN[1:plot_window+1]
        #y_varT = y_varT[1:plot_window+1]
        #y_varP = y_varP[1:plot_window+1]


	#get the right data in the lines
	#Do I need to do something with the color here too? and the splitting the data???

#        colTemp = colTemp.to_numpy() #turn it into an array....
#        print(colTemp)
#        lineC.set_ydata(df.loc[:, "conduS"])
#        lineN.set_ydata(df.loc[:, "turbNTU"])
#        lineT.set_ydata(df.loc[:, "tempC"])
#        lineP.set_ydata(df.loc[:, "pH"])



#        df= pd.read_csv('seeBoat.csv')
        df= pd.read_csv(path)

#print(len(df.index))
        df["dataPt"]=range(1,len(df.index)+1)

        print(len(df.index))

        df1 = df.rename(columns={'tempC':'S1', 'conduS':'S2', 'pH':'S3', 'turbNTU':'S4'}).reset_index()
        df_long=pd.wide_to_long(df1, stubnames=['S'], i='index', j='id').reset_index()

#[['sensor', 'boatName', 'GPSlong', 'GPSsec', 'date', 'GPShour', 'GPSms', 'GPSlat', 'GPSmin', 'deviceID', 'time', 'dataPt', 'value']]

#print(df_long)
#print(list(df_long.columns.values))
#print(list(df_long)) #get the column names

        df_long = df_long.replace({'id':{1:"tempC", 2:"conduS", 3:"pH", 4:"turbNTU"}})

        ax=sns.relplot(x="dataPt", y="S", hue="boatName", kind='line', col='id', data=df_long, facet_kws={'sharey': False, 'sharex': True}) 

#Gets rid of the xaxis labels
        ax.set(xticklabels=[])


#plt.show()
        plt.draw()
        plt.pause(3)
        plt.close()









        print("test1")
#        lineP.set_ydata(y_varP)

	#rescale
#        plotfix(ax[0,0])
#        plotfix(ax[0,1])
#        plotfix(ax[1,0])
#        plotfix(ax[1,1])
        print("test2")
#        fig.legend(loc="lower right")

#        fig.canvas.draw() 
        print("test3")
#        fig.canvas.flush_events()
        print("test4") 
#        plt.pause(1) #added this via website to make sure we see the plot
    except:
        print("Keyboard Interrupt")
        break
