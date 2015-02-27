Arduino library for AD5161 I2C/SPI digital potentiometer
==============

	v1.0 - First release

This potentiometer has 256 steps, VDD 2,7-5,5V, exists with R=5K, 10K, 50K, 100K

This device can be driven using either I2C or SPI buses. This library supports both busses.


[AD5161](http://www.analog.com/static/imported-files/data_sheets/AD5161.pdf)'s Analog Device page.

## Features ##
- Move to a positon straight, by step or by step and time
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

## Documentation ##

	ANALOGDEVICE_AD5161(void)
			Create object for device in SPI mode, all other data got from #define section of header file

	ANALOGDEVICE_AD5161(boolean SpiI2c_mode, uint8_t cs_pin, uint8_t ad0_value)
			Create object for device over SPI or I2C, with a way to specify CS pin and AD0 value for I2C address.  All other data got from #define section of header file
			Set unused variables to 0

	ANALOGDEVICE_AD5161(boolean SpiI2c_mode, uint8_t cs_pin, uint8_t ad0_value, uint8_t nb_step, uint16_t R_value)
			Fully speccified device. Set unused variables to 0
	
	init()
			Used to init the object. This is mandatory. As default the debug mode is disabled
	
	update()
			This function must be called in the loop function of your sketch to allow proper step by step working
	
	moveTo_byStep(int8_t position, int8_t step_size)
			Move to a specified position with a step size
			A 0 step size means a straight move
	
	moveTo_byTime(int8_t position, int8_t step_size, unsigned long min_time)
			Move to a specified position with a step size. Each step is done every min_time
			
	moveBy(int decalage, uint8_t mode)
			Move from current position to position + decalage
			Step mode = 1, Time mode = 2
	
	setMidscale()
			Set stepper in the middle of the potentiometer
	
	getPosition()
			Read position from device in I2C mode
			Return last known positon in SPI mode
	
	getRvalue()
			Calculated R value according to position
	
	getOpMode()
			Return Operation mode
			0: stopped, 1:ongoing move by step, 2: ongoing move by time
	
	enable_debug (boolean status)
			Start or stop Serial debug output
			status: true = output activated
			status: false = output stopped

