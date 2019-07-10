Laura Perovich: SeeBoat control board for all the sensors 

--third board layer to be used with Featherwing ultimate GPS and Feather M0 LoRa
--see github parts list in hardware folder for parts details

######### Update 7/3/19
Fixed the labels on the LED strand connector

######### Update 6/23/18
Removed on board LED, spaced out the headers to better fit all the parts and the labels
Included DotStar LED strand connector
Water sensor to tell if it's in water is unnecessary, not implemented

######### Update 5/25/17
Combines the fishet-0.1 board with GPS to works as a breakout board for the feather M0 w/LoRa

parts from the fishnet to include:
*tricolor LED (3 pwm pins, other sides to ground)
*on/off switch for board
*temperature sensor connector (SDA, SCL, GND, PWR)
*turbidity sensor connector
*conductivity sensor connector
*water sensor (to tell if it's in water or not) [UNNECESSARY, NOT IMPLEMENTED]

GPS featherwing includes:
*GPS
*clock via GPS

#############################################
ORIGINAL BOARD LAYOUT:
# Adafruit-Ultimate-GPS-FeatherWing-PCB
PCB files for the Adafruit Ultimate GPS FeatherWing
Format is EagleCAD schematic and board layout
For more details, check out the product page at
   * https://www.adafruit.com/products/3133
Designed by Adafruit Industries.  
Creative Commons Attribution, Share-Alike license, check license.txt for more information
All text above must be included in any redistribution
