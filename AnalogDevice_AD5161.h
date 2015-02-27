/**************************************************************************/
/*! 
    @file     AnalogDevice_AD5161.h
    @author   SOSAndroid.fr (E. Ha.)
	
    @section  HISTORY

	v1.0 - First release


    This library aims to interact with the AD5161 from Analog Digital. This works through I2C or SPI
	This should work on other 1 channel devices from the manufacturer
	
	Does not support 1024 position devices nor daisy chaining
	
    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2015, SOSAndroid.fr (E. Ha.)
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/
#ifndef _ANALOGDEVICE_AD5161_H_
#define _ANALOGDEVICE_AD5161_H_

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif
#include <Wire.h>
#include <SPI.h>

//Default values
#define AD5161_RVALUE 5000 //5K chip
#define AD5161_I2C_ADDR_BASE 0x16 //6 digits address base. AD0 value will add the 7th digits
#define AD5161_STEPS  255 //number of steps of the R ladder
#define AD5161_CS_PIN 13
#define AD5161_0_VALUE 60 //at 0x00 position (pin B side), minimum 60R
#define AD5161_I2C_RESETMIDSCALE 0x40
#define AD5161_I2C_RESETTOZERO 0x20
#define AD5161_I2C_NORMALWRITE 0x00

// Enabling debug - comment 1st line to disable debug mode at compiling
#define SERIAL_DEBUG
#define DEBUG_SERIALBAUD 9600
#define DEBUG_COMMODE 0
#define DEBUG_CHIPADDR 1
#define DEBUG_CSPIN 2
#define DEBUG_OPMODE 3
#define DEBUG_STEPSIZE 4
#define DEBUG_TIMEFRAME 5
#define DEBUG_STEPPERPOSITION 6
#define DEBUG_DEBUGSTATUS 7
#define DEBUG_ALL 255

//to start SPI or I2C bus from the object if only one device is used in the hardware setup
#define BUSES_INIT 

//Operation constants
#define OP_STOP 0
#define OP_STEP_MODE 1
#define OP_TIME_MODE 2
#define OP_DEFAULT_STEP_SIZE 1
#define OP_DEFAULT_TIME_FRAME 100 //ms


class ANALOGDEVICE_AD5161 {
 public:
	ANALOGDEVICE_AD5161(void);
	ANALOGDEVICE_AD5161(boolean SpiI2c_mode, uint8_t cs_pin, uint8_t ad0_value);
	ANALOGDEVICE_AD5161(boolean SpiI2c_mode, uint8_t cs_pin, uint8_t ad0_value, uint8_t nb_step, uint16_t R_value);
	
	void			init(void);
	void			update(void);
	void			moveTo_byStep(int8_t position, int8_t step_size);
	void			moveTo_byTime(int8_t position, int8_t step_size, unsigned long min_time);
	void			moveBy(int decalage, uint8_t mode);
	void			setMidscale(void);
	uint8_t			getPosition(void);
	int				getRvalue(void);
	uint8_t			getOpMode(void);
	void			enable_debug (boolean status);
	
 private:
	//constants
	boolean			_I2C_mode;
	uint8_t			_chipAddress;
	uint8_t			_CS_pin;
	unsigned int	_chip_maxvalue;
	int				_step_Rvalue;
	int				_step0_Rvalue;
	uint8_t			_stepper_maxposition;

	//working values
	uint8_t			_stepper_currentposition;
	uint8_t			_stepper_target;
	uint8_t			_stepper_stepsize;
	unsigned long	_stepper_updateTimeframe;
	uint8_t			_OP_mode;
	unsigned long	_lastmillis;
	boolean			_debug_enabled;
	
	
	void 			writeI2C(uint8_t position);
	void			writeSPI(uint8_t position);
	uint8_t			readI2C(void);
	void			update_byStep(void);
	void			update_byTime(void);
	void			debug_serial(uint8_t dataCode);
	
};

#endif