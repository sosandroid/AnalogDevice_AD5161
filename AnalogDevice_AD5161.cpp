/**************************************************************************/
/*!
    @file     AnalogDevice_AD5161.cpp
    @author   SOSAndroid.fr (E. Ha.)
	
    @section  HISTORY

	v1.0 - First release


    This library aims to interact with the AD5161 from Analog Digital. This works through I2C or SPI
	This should work on other 1 channel devices from the manufacturer
*/
/**************************************************************************/

#include <stdlib.h>
#include <Wire.h>
#include <SPI.h>
#include "AnalogDevice_AD5161.h"

/*========================================================================*/
/*                            CONSTRUCTORS                                */
/*========================================================================*/

/**************************************************************************/
/*!
    Constructors
*/
/**************************************************************************/
ANALOGDEVICE_AD5161::ANALOGDEVICE_AD5161(void) 
{
	//default : SPI mode
	_I2C_mode = false;
	_chipAddress = 0;
	_CS_pin = AD5161_CS_PIN;
	_stepper_maxposition = AD5161_STEPS;
	_chip_maxvalue = AD5161_RVALUE;
}

ANALOGDEVICE_AD5161::ANALOGDEVICE_AD5161(boolean SpiI2c_mode, uint8_t cs_pin, uint8_t ad0_value) 
{
	//SpiI2c_mode : False = SPI, True = I2C
	//ad0_value within 0-1 range
	
	_I2C_mode = SpiI2c_mode;
	_chipAddress = (AD5161_I2C_ADDR_BASE << 1);
			if (ad0_value < 2)  _chipAddress += ad0_value;
	_CS_pin = cs_pin;
	_stepper_maxposition = AD5161_STEPS;
	_chip_maxvalue = AD5161_RVALUE;

}

ANALOGDEVICE_AD5161::ANALOGDEVICE_AD5161(boolean SpiI2c_mode, uint8_t cs_pin, uint8_t ad0_value, uint8_t nb_step, uint16_t R_value) 
{
	//SpiI2c_mode : False = SPI, True = I2C
	//ad0_value within 0-1 range
	
	_I2C_mode = SpiI2c_mode;
	_chipAddress = (AD5161_I2C_ADDR_BASE << 1);
			if (ad0_value < 2)  _chipAddress += ad0_value;
	_CS_pin = cs_pin;
	_stepper_maxposition = nb_step;
	_chip_maxvalue = R_value;

}

/*========================================================================*/
/*                           PUBLIC FUNCTIONS                             */
/*========================================================================*/

void ANALOGDEVICE_AD5161::init(void) {

	#ifdef SERIAL_DEBUG
		if(!Serial) {
			Serial.begin(DEBUG_SERIALBAUD);
		}
	#endif

	#ifdef BUSES_INIT
		(_I2C_mode) ? Wire.begin() : SPI.begin();
	#endif

	
	_step_Rvalue = _chip_maxvalue / (_stepper_maxposition + 1);
	_step0_Rvalue = AD5161_0_VALUE;
	_stepper_currentposition = 0;
	_OP_mode = OP_STOP;
	_stepper_stepsize = OP_DEFAULT_STEP_SIZE;
	_stepper_updateTimeframe = OP_DEFAULT_TIME_FRAME;
	
	if(!_I2C_mode) {
		pinMode(_CS_pin, OUTPUT);
		digitalWrite(_CS_pin, HIGH);
	}
	ANALOGDEVICE_AD5161::debug_serial(DEBUG_ALL);
	_debug_enabled = false;
	
	ANALOGDEVICE_AD5161::setMidscale();

	return;
}

void ANALOGDEVICE_AD5161::update(void) {

	switch(_OP_mode) {
		case OP_STEP_MODE:
			ANALOGDEVICE_AD5161::update_byStep();
			break;
		case OP_TIME_MODE:
			ANALOGDEVICE_AD5161::update_byTime();
			break;
		default:
			break;
	}
	return;
}

void ANALOGDEVICE_AD5161::setMidscale(void) {
	
	_stepper_currentposition = (_stepper_maxposition >> 1);
	
	if (_I2C_mode) {
		ANALOGDEVICE_AD5161::writeI2C(_stepper_currentposition);
	}
	else {
		ANALOGDEVICE_AD5161::writeSPI(_stepper_currentposition);
	}
	
	return;
}

uint8_t ANALOGDEVICE_AD5161::getPosition(void) {
	if (_I2C_mode) {
		_stepper_currentposition = ANALOGDEVICE_AD5161::readI2C();
	}
	else {
		//means SPI mode, non readable
	}
	return _stepper_currentposition;
}

int ANALOGDEVICE_AD5161::getRvalue(void) {
	//provides a rounded value of the current resistor stating from B pin of the device
	return (int)((_stepper_currentposition * _step_Rvalue) + _step0_Rvalue);
}

void ANALOGDEVICE_AD5161::moveTo_byStep(int8_t position, int8_t step_size) {

	if (_OP_mode == OP_TIME_MODE) return; //We are not changing opration mode during ongoing operations

	//a null step size means moving straight to the desired position
	_stepper_target = position;
	if (step_size == 0) {
		if (position >= _stepper_currentposition) {
			_stepper_stepsize = position - _stepper_currentposition;
		}
		else {
			_stepper_stepsize = _stepper_currentposition - position;
		}
	}
	else {
		_stepper_stepsize = step_size;
	}
	_OP_mode = OP_STEP_MODE;
	
	ANALOGDEVICE_AD5161::debug_serial(DEBUG_STEPPERPOSITION);
	ANALOGDEVICE_AD5161::debug_serial(DEBUG_OPMODE);
	ANALOGDEVICE_AD5161::debug_serial(DEBUG_STEPSIZE);

	
	ANALOGDEVICE_AD5161::update();
	return;
}

void ANALOGDEVICE_AD5161::moveTo_byTime(int8_t position, int8_t step_size, unsigned long min_time) {
	
	if (step_size == 0) return; //we cannot move with a step size null and a minimum delay per step. Please use ANALOGDEVICE_AD5161::moveTo_bystep instead
	if (_OP_mode == OP_STEP_MODE) return; //We are not changing opration mode during ongoing operations
	
	_stepper_target = position;
	_stepper_stepsize = step_size;
	_stepper_updateTimeframe = min_time;
	_OP_mode = OP_TIME_MODE;
	
	ANALOGDEVICE_AD5161::debug_serial(DEBUG_STEPPERPOSITION);
	ANALOGDEVICE_AD5161::debug_serial(DEBUG_OPMODE);
	ANALOGDEVICE_AD5161::debug_serial(DEBUG_STEPSIZE);
	ANALOGDEVICE_AD5161::debug_serial(DEBUG_TIMEFRAME);
	
	ANALOGDEVICE_AD5161::update();
	return;
}

void ANALOGDEVICE_AD5161::moveBy(int decalage, uint8_t mode) {

	//uses the last known values for _stepper_stepsize & _stepper_updateTimeframe
	
	uint8_t target;
	int var = (int) _stepper_currentposition + decalage;
	if (var < 0 ) {
		target = 0;
	}
	else if (var >= 0 && var <= (int) _stepper_maxposition) {
		target = (uint8_t) var;
	}
	else {
		target = _stepper_maxposition;
	}
	
	switch (_OP_mode) {
		case OP_STOP: 
			if (mode == OP_STEP_MODE) ANALOGDEVICE_AD5161::moveTo_byStep(target, _stepper_stepsize);
			if (mode == OP_TIME_MODE) ANALOGDEVICE_AD5161::moveTo_byTime(target, _stepper_stepsize, _stepper_updateTimeframe);
			//in other cases do nothing as this not a known mode
			break;
		case OP_STEP_MODE: 	case OP_TIME_MODE:
			_stepper_target = target;
			break;
		default:
			break;
	}

	return;
}
uint8_t ANALOGDEVICE_AD5161::getOpMode(void) {
	return _OP_mode;
}

void ANALOGDEVICE_AD5161::enable_debug (boolean status) {
	#ifdef SERIAL_DEBUG
		//to avoid debug enabling if Serial not available
		if (status) {
			_debug_enabled = true;
		}
		else {
			_debug_enabled = false;
		}
		ANALOGDEVICE_AD5161::debug_serial(DEBUG_DEBUGSTATUS);
	#endif
	return;
}


/*========================================================================*/
/*                           PRIVATE FUNCTIONS                            */
/*========================================================================*/

void ANALOGDEVICE_AD5161::writeI2C(uint8_t position) {

	if(_I2C_mode) {
		Wire.beginTransmission(_chipAddress);
		Wire.write(AD5161_I2C_NORMALWRITE);
		Wire.write(position);
		Wire.endTransmission();
	}
	ANALOGDEVICE_AD5161::debug_serial(DEBUG_STEPPERPOSITION);
	return;
}

uint8_t ANALOGDEVICE_AD5161::readI2C(void) {
	
	uint8_t nb_byte = 1;
	
	if(_I2C_mode) {
		Wire.requestFrom(_chipAddress, nb_byte);
		return Wire.read();
	}
	ANALOGDEVICE_AD5161::debug_serial(DEBUG_STEPPERPOSITION);
	return 0;
}

void ANALOGDEVICE_AD5161::writeSPI(uint8_t position) {

	if(!_I2C_mode) {
		digitalWrite(_CS_pin, LOW);
		SPI.transfer(position);
		digitalWrite(_CS_pin, HIGH);
	}
	ANALOGDEVICE_AD5161::debug_serial(DEBUG_STEPPERPOSITION);
	return;
}

void ANALOGDEVICE_AD5161::update_byStep(void) {

	uint8_t nextstep;
	
	//calculating next step to reach
	if (_stepper_currentposition < _stepper_target) {
		//we are increasing stepper
		if ((_stepper_currentposition + _stepper_stepsize) >= _stepper_maxposition) {
			//We are over the top limit
			_OP_mode = OP_STOP;
			nextstep = _stepper_maxposition;
		}
		else if ((_stepper_currentposition + _stepper_stepsize) >= _stepper_target) {
			//we are over the required position
			_OP_mode = OP_STOP;
			nextstep = _stepper_target;
		}
		else {
			//Still some work to do
			nextstep = _stepper_currentposition + _stepper_stepsize;
		}
	}
	else {
		//we are decreasing stepper
		if (_stepper_stepsize >= _stepper_currentposition) {
			//We are trying to go below 0
			_OP_mode = OP_STOP;
			nextstep = 0x00;
		}
		else if ((_stepper_currentposition - _stepper_stepsize) <= _stepper_target) {
			//we are going below the required position
			_OP_mode = OP_STOP;
			nextstep = _stepper_target;
		}
		else {
			//Still some work to do
			nextstep = _stepper_currentposition - _stepper_stepsize;
		}
	}
	
	//moving to next step
	if(_I2C_mode) {
		ANALOGDEVICE_AD5161::writeI2C(nextstep);
	}
	else {
		ANALOGDEVICE_AD5161::writeSPI(nextstep);
	}
	return;
}

void ANALOGDEVICE_AD5161::update_byTime(void) {
	if (millis() >= (_lastmillis + _stepper_updateTimeframe)) {
		ANALOGDEVICE_AD5161::update_byStep();
		_lastmillis = millis();
	}
	return;
}

void ANALOGDEVICE_AD5161::debug_serial(uint8_t dataCode) {
	#ifdef SERIAL_DEBUG
	if (_debug_enabled) {
		switch (dataCode) {
			case DEBUG_COMMODE:
				Serial.print("communication mode : ");
				(_I2C_mode) ? Serial.println("I2C") : Serial.println("SPI");
				break;
			case DEBUG_CHIPADDR:
				Serial.print("I2C chip address : ");
				(_I2C_mode) ? Serial.println(_chipAddress, HEX) : Serial.println("Error, SPI mode");
				break;
			case DEBUG_CSPIN:
				Serial.print("SPI CS PIN : ");
				(_I2C_mode) ? Serial.println("Error, I2C mode") : Serial.println(_CS_pin, DEC);
				break;
			case DEBUG_OPMODE:
				Serial.print("Operation mode : ");
				if (_OP_mode == OP_STOP) Serial.println("stepper not moving");
				if (_OP_mode == OP_STEP_MODE) Serial.println("stepper moving by steps");
				if (_OP_mode == OP_STOP) Serial.println("stepper moving by steps and by time");
				break;
			case DEBUG_STEPSIZE:
				Serial.print("Step size : ");
				Serial.println(_stepper_stepsize, DEC);
				break;
			case DEBUG_TIMEFRAME:
				Serial.print("Timeframe size : ");
				Serial.print(_stepper_updateTimeframe, DEC);
				Serial.println(" ms");
				break;
			case DEBUG_STEPPERPOSITION:
				Serial.print("Stepper current position : ");
				Serial.print(_stepper_currentposition, DEC);				
				break;
			case DEBUG_DEBUGSTATUS:
				Serial.print("Debug is ");
				(_debug_enabled) ? Serial.println("on") : Serial.println("off");
			default:
				//means all debug data to serial
				ANALOGDEVICE_AD5161::debug_serial(DEBUG_COMMODE);
				ANALOGDEVICE_AD5161::debug_serial(DEBUG_CHIPADDR);
				ANALOGDEVICE_AD5161::debug_serial(DEBUG_CSPIN);
				ANALOGDEVICE_AD5161::debug_serial(DEBUG_STEPSIZE);
				ANALOGDEVICE_AD5161::debug_serial(DEBUG_TIMEFRAME);
				ANALOGDEVICE_AD5161::debug_serial(DEBUG_STEPPERPOSITION);
				ANALOGDEVICE_AD5161::debug_serial(DEBUG_DEBUGSTATUS);
				Serial.println("--");
				break;
		}
	}
	#endif
	return;
}