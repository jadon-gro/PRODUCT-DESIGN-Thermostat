# PRODUCT-DESIGN-Thermostat
## Introduction
This is a personal project as a self study for product design (CAD, IoT, Arduino ect.).
### Problem
The thermostat in the Georgia Tech apartment I live in is ancient and horrible. It is just a knob labelled "hotter" and "colder." I wanted to make a device that probes many locations in the apartment and can automatically articulate the knob based on the average temperature.

### Design Goals
I wanted to use a hub that displays:
- The temperature/humidity of the apartment (gateway that can recieve data from many nodes)
- The target temperature
- The current mode (cooling, warming, or idle)

I also wanted satallite nodes capable of sending temperature/humidity data.

## Hardware
### Hub
1. ESP-32s - https://www.amazon.com/gp/product/B0718T232Z/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1
2. Adafruit Monochrome OLED - https://www.adafruit.com/product/326
3. Generic Breadboard Buttons
4. Hatchbox White 1.75mm PLA - https://www.amazon.com/gp/product/B00J0GMMP6/ref=ppx_yo_dt_b_asin_title_o06_s00?ie=UTF8&psc=1
I used an ESP32 as the hub microcontroller for its bluetooth capabilities which I might use later to create a bluetooth app (see future iterations). The faster processor and greater I/O are just bonuses.
