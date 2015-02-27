Arduino library for AD5161 I2C/SPI digital potentiometer
==============

	v1.0 - First release

This device can be driven using either I2C or SPI buses. This library supports both busses.
This potentiometer has 256 steps, VDD 2,7-5,5V, exists with R=5K, 10K, 50K, 100K

[AD5161](http://www.analog.com/static/imported-files/data_sheets/AD5161.pdf) Analog Device page.

## Features ##
- Move to a positon by step or by step and time
- Move by a differential value
- Get current stepper position
- Get current operation mode (stopped, by step, by time)
- Set at mid scale
- Enabling / disabling debug to Serial output

## Example ##
A simple voltage divider moving along the full scale of the potentiometer. The Serial output is the stepper position read from the lib and extrapolated from AnalogRead()


## Testing ##
- Tested only against AD5161BRM5
- Tested on Arduino Mega with Arduino IDE 1.0.5
- Please comment about other devices

