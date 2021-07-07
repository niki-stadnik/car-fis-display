# Car FIS Display
I needed a few sensors for diagnostic data on my car, but just adding more gauges clutters the interior and i didn't want that. So when one of my sensors failed, I decided that instead of buying a new one I could build a custom diplay with all the sensors I want for tha same price as the replacement. :P

## Features
* Warning lights
* Sensors
* Clock
* Dimming

## Warning Lights
I didn't want to trade warning lights for sensors plus I wanted to keep the factory design, so for now they are absolutely the same.

## Sensors
I removed the oil pressure gauge and the failed oil temperature gauge, and installed a display with the two sensors mentont plus voltage sensor and water temperature sensor.
Why water temperature sensor since the factory instrument cluster clearly has one? Because they are made from the factory to show 90°C in the range from 70°C to 120°C as to not stress the average driver, and i wanted an accurate one.

## Clock
I added a clock since the on board computer can show only one reading at a time, and I prefer to see the fuel consumption.

## Dimming
And finally i decided to add dimming, so the display wont blind me when driving at night. The arduino is connected to the car's dimmer circuit, and the display is dimming along with all the other lights on the dashboard.

## Future tasks
I added a MCP2515 module to the arduino in order to connect to the car's CAN BUS. That should be able to give all the diagnostic data that can be gained with a diagnostic PC.

Adding more sensors.


## Pictures

Stock instrument cluster and warning lights

The stock cluster of the mk4 golf i have, has only 6 warning lights at the center, and that is a perfect spot for adding stuff.

<img src="pics/20210223_124024.jpg" width="400"> <img src="pics/20200901_143025.jpg" width="400">



New instrument cluster and warning lights

I wanted to keep the factory layout of the warning lights for the time being so i left that part of the display reserved only for them. But in the future that can change.

<img src="pics/20210622_144329.jpg" width="400"> <img src="pics/20210622_144449.jpg" width="400">



The new instrument cluster with the car working and warmed up.

<img src="pics/20210622_145829.jpg" width="1000">



The testing phase and schematic.

Here the sensors are being calibrated on a small Arduino pro micro. Later switched to Arduino Mega for the finished project.

<img src="pics/20210206_180025.jpg" width="400"> <img src="pics/20210210_014952.jpg" width="400">



The backside of the instrument cluster and a board with all the connections for the arduino, made as a custom shield.
The warning lights have constant 12v through them and the connection to the ground is controlled by the ECU. So I had to add a small board with a resistor and photocoupler for each of the 6 leds.

<img src="pics/20210304_161419.jpg" width="400"> <img src="pics/20210306_152540.jpg" width="400">

## For the project
I used arduino mega because i needed a lot of io pins. ds3231 rtc module for the clock in order to have an accurate and long lasting clock even in case of flat battery.
A voltage regulator to drop from 12v to 9v and keep it steady. A nextion display because it has it's own software and therefore is easier to control with arduino.
And lastly mcp2515 module for CAN BUS expansions.
