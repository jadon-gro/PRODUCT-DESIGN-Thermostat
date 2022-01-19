# PRODUCT-DESIGN-Thermostat

<img src="https://user-images.githubusercontent.com/71471706/150188091-5cfa9878-7da0-4dd7-9eea-58ab913f1a07.jpg" width="400" height="300">


## Introduction
This is a personal project as a self study for product design (CAD, IoT, Arduino ect.).
### Problem
The thermostat in the Georgia Tech apartment I live in is ancient and horrible. It is just a knob labelled "warmer" and "cooler." I wanted to make a device that probes many locations in the apartment and can automatically articulate the knob based on the average temperature.

### Design Goals
I wanted to use a hub that displays:
- The temperature/humidity of the apartment (gateway that can recieve data from many nodes)
- The target temperature
- The current mode (cooling, warming, or idle)

I also wanted satallite nodes capable of sending temperature/humidity data.

## Hardware
### Hub
1. ESP-32s Microcontroller - https://www.amazon.com/gp/product/B0718T232Z/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1
2. Adafruit Monochrome OLED - https://www.adafruit.com/product/326
3. Generic Breadboard Buttons
4. Hatchbox White 1.75mm PLA - https://www.amazon.com/gp/product/B00J0GMMP6/ref=ppx_yo_dt_b_asin_title_o06_s00?ie=UTF8&psc=1
5. Generic Microservo - (I used a really old radioshack one I found in my drawer - I have the same one from many AIBs)
I used an ESP32 as the hub microcontroller for its bluetooth capabilities which I might use later to create a bluetooth app (see future iterations). The faster processor and greater I/O are just bonuses.

### Probe
1. ESP-8266 Microcontroller - https://www.amazon.com/gp/product/B010O1G1ES/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&th=1
2. SHT31-D Temperature and Humidity Sensor - https://www.amazon.com/gp/product/B085WBFCMF/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1
The SHT31-D was chosen because it's got an incredible .3 degree celsius error range, has an integrated heater for getting rid of moisture and is relatively cheap.
Other options from my local microcenter had a many-degree range of error which was unacceptable for my use.

## Design
The 3D model underwent a LARGE number of prototypes and test-prints for fit and ease of use. I settled on a top friction mounted unibody that reaches around to the front where the servo is screwed in place.

The model attaches to the thermostat knob using an independent friction fitted cap that has a slot for the microservo to grab.

Buttons were a difficult part to mount as they are more meant for breadboards. The best solution I found was mounting them on the top leaving holes for the pins to be accessable from the inside.

The model is viewable in the src folder.

## Assembly
### Printing
<img alt="Cura Slicer Screenshot - Body" src="https://user-images.githubusercontent.com/71471706/150189884-57fc8708-c8f2-4465-9d97-66a99b20b3fb.png" width="300" height="300"> <img alt="Cura Slicer Screenshot - Back" src="https://user-images.githubusercontent.com/71471706/150190739-ca94a50f-7067-49b9-9a26-6def3da9446d.png" width="300" height="300"> <img alt="Cura Slicer Screenshot - Back" src="https://user-images.githubusercontent.com/71471706/150191369-e5c106c3-2f6e-4a4a-891b-88087d23ae38.png" width="300" height="300">

Body:
I like to print the whole model in one run using supports and 20% infill (for speed). The way to get the best finish on viewable parts, best bed adhesion, and easy support removal is printing it from the back to the front.

Backplate:
Print the back facing down, with NO supports, and 100% infill. You don't want the clips snapping off and it's a small part so the time save from less infill is minimal.

Knob:
Print with opening facing down using supports. Infill is up to you. This allows for the best finish on the visible portions of the part and a rougher and grippier texture where the part touches the knob.

### Soldering
Assembly from here on is more intuitive and you can probably figure out what works best for your tools and abilities. I will however provide some tips for a few key parts.

PinOuts for the code I wrote:
Hub:
| Connection | ESP-32s GPIO Pin |
| ---------- | ---------------- |
| Servo PWM | Pin 13 |
| Cold Button | Pin 34 |
| Warm Button | Pin 35 |
| Wake Button | Pin 32 |
| OLED MOSI | 23 |
| OLED CLK | 18 |
| OLED DC | 19 |
| OLED CS | 5 |
| OLED Reset | 17 |

Buttons:
First break off one of the pins for two of the buttons. The buttons on either end have an unused pin that conflicts with the mounting holes. Glue the bottom of the buttons to the slots on the main body and have the pins come through to the inside. Now you can solder all the pins (one side high, the other side to corresponding GPIO pin).

<img src="https://user-images.githubusercontent.com/71471706/150201114-a79a0e03-a5f7-4003-99d1-f300d8b0f652.jpg" width="320" height="300">

Now you can solder appropriate length wire from each component to the microcontroller. I glued the rest of the parts in place with hot glue. Make sure to align the port with the hole and route the servo wires through the smaller hole.


Probe:
Just connect the SHT31 onto the I2C bus of your microcontroller.

I may make an encolsure for this in the future but for now I just have mine on a breadboard on my desk.

<img src="https://user-images.githubusercontent.com/71471706/150201590-370a45d6-4455-48c1-bd8a-0c9a6cefd5a7.jpg" width="320" height="300">


## Things that still need doing/fixing
1. Proper graphical support for more than one probe
2. Making an enclosure for the probe
3. Making backplate pins that don't interfere with the buttons
4. Making the system adjustable over bluetooth or a web-app (difficult over student wifi)

## Known issues
- My system randomly reboots sometimes. I think this may have to do with a poor soldering job on my part and pins shorting. I'm working on figuring that out.


If you have any questions about code/design decisions, please feel free to reach out to me via email: jadon.gro@gatech.edu






