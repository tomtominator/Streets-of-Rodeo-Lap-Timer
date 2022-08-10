# Streets-of-Rodeo-Lap-Timer

## Description  

This project is a motion detecting lap timer with a custom built 4 digit 7 segement display for the timer. 

The initial idea was to use a laser sensor that would trigger when we passed through. It wasn't ideal to have to place objects on both sides and possibly even have a wire tracing back over the track. I remembered using sonic distance sensors in physics class which only had 1 emitter and were suprisingly accurate. We happened to have a ultrasonic distance sensor and microcontroller lying around so we built the project around that. 

The operation of the distance sensor is slightly complicated but luckily there is plenty of code on the internet for this sensor. This is the explanation we used and base code we modified: https://create.arduino.cc/projecthub/abdularbi17/ultrasonic-sensor-hc-sr04-with-arduino-tutorial-327ff6

We modeled our custom 7 segment display off of this one we bought. It is a common anode display. This datasheet was useful: https://www.allelectronics.com/mas_assets/media/allelectronics2018/spec/FDA-5.pdf

## Display

### Circuit Diagrams
  
![Segment Diagram](Diagrams/Segment-Cir-Diagram.JPG)  

Above is the circuit diagram for a given segment. We will be running the LEDS at 20mA and controlling the segments with a PNP transistor switched by the launchpad. The 31 ohms was calculated to get the desired 20mA/LED number.

![Digit Diagram](Diagrams/Digit-Cir-Diagram.JPG)   

The 5mA current coming into the board per transistor is calculated using the hfe number provided by the transistor datasheet and this equation: I_C = (hfe) * I_B. For a PNP we are sinking current. The total current into the board is 35mA. The board must only dissapate about .175W. Note that only 1 digit is ever on at a time. The total draw of the digit from the power source is 160mA * 7 = 1.12A --at 5V--> 5.6W 
  
### Experimental Results

- Total segment current = 100mA - less than expected  
- Current into base of 2N3905 = 3.5mA  
- Transistor base resistance: 150 || 270 = 96 ohms  
- No need for resistors for each LED, the transistor base resistor on limits the current flow from emitter to collector  
- LEDS plenty bright at this power level, likely want to be able to reduce brightness with pot somehow

These results will give a general estimate to make sure we don't exceed any of our parts power limits. This is also a sanity check that calculations were done correctly.  

### STL Files Descriptions

- 1 Segment File
Includes 8 holes for each led to be inserted and soldered on the back

- Slip in Plate
Accepts 1 segment plate to slip in firmly, allowing space for soldered connections. Wires are to be routed through the center hole in the back. Other 2 holes used for mounting to final backplate.

- 1 digit linked
Not included in our final design but would save alot of time and effort. Instead of printing segments individually this file includes 7 connected with proper spacing. Recommended for printers with ABL or a very nicely leveled bed. 

### Assembly
After 8 LEDs are soldered in paralell in each segment, power wires can be routed through the slip in plate, and the segment can be firmly inserted. (If using the individual plates, each should be mounted to a backplate prior to doing this.) 

Each segment with LEDs and power wires are pressed into their plates and all wires are routed through the slip in plate and through holes drilled in the backplate. All wires are easily accesable in the backplate for later connections to transistors and board. 

Segments could be painted matt black for a cleaner look.

All switches and pots can be mounted on the backplate 

# Final Design

We had to use a voltage regulator to provide both 5 and a variable 3.3-2.3V from the 5V power supply. We used the LM317 variable voltage regulator with a 10K potentionmeter. This explanation video was extremely helpful: https://www.youtube.com/watch?v=IjJWWGPjc-w.  

![Voltage Regulator Diagram](Diagrams/VoltageRegulatorDiagram.JPG)   

Our 7 segment display was slightly different from the standard design to make wiring easier. However, this made the pinout quite busy:  

![Custom 7 Segment Display Pinout](Diagrams/7SegDisplayPinout.JPG)   

Luckily the launchpad has many i/o pins so this simplified our hardware and lighting up a sigle digit at a time (done this way to save on power and prevent too much current into the launchpad) could be done in the code.  
The distance sensor was connected to the launchpad as well. The whole assembly was controlled by c++ code run on the launchpad, switching all the segments and operating the distance sensor.

## Wooden Case and Sensor Case (Penguin)

![Picture of box and punguin sensor](images/PXL_20220809_234259224.MP.jpg)

## Code

The final version of the [code on the Launchpad](https://github.com/tomtominator/Streets-of-Rodeo-Lap-Timer/blob/main/LaunchPad%20Code/Lap_Time_Display_Custom_Final/Lap_Time_Display_Custom_Final.ino).  

Possible Future Improvements:  
Make main loop event driven by performing certain functions every amount of millis() difference, instead of using counters to trigger actions. (This way we actually sample every 200ms for example)  
Make one of the digits a lap counter since we don't have hundreds place precision anyway


## Ingredients:  

- HC-SR04 Ultrasonic Distance Sensor: https://www.amazon.com/SainSmart-HC-SR04-Ranging-Detector-Distance/dp/B004U8TOE6 
- 4-Digit 7 Segement Display (Deprecated): https://www.allelectronics.com/item/fda-5/4-digit-red-clock-display-0.4-digits/1.html  
- TI Launchpad MSP-EXP430F5529LP: https://www.ti.com/tool/MSP-EXP430F5529LP
- 232 Red LEDS: https://www.allelectronics.com/item/led-513/5mm-red-led/1.html
- 30 PNP Transistors: https://www.allelectronics.com/item/2n3905/2n3905-pnp-to-92-transistor/1.html
- LM317 Voltage Regulator
- Switch, 10K pot, lots of resistors(mostly 270ohm)

### Spec/Datasheets:  

- Transistors: https://www.mouser.com/datasheet/2/308/FairchildSemiconductor_16142055386277-1191860.pdf
- LEDS: https://www.allelectronics.com/mas_assets/media/allelectronics2018/spec/LED-513.pdf
- Launchpad: https://www.ti.com/lit/ug/slau533d/slau533d.pdf?ts=1653799657399
- Sensor: https://web.eece.maine.edu/~zhu/book/lab/HC-SR04%20User%20Manual.pdf
- Display: https://www.allelectronics.com/mas_assets/media/allelectronics2018/spec/FDA-5.pdf
- Voltage Regulator: https://www.ti.com/document-viewer/LM317A/datasheet/features-snvsac20075#SNVSAC20075


Daniel and Thomas Matthew 2022
