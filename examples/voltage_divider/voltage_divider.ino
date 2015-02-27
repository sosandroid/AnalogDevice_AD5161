/**************************************************************************/
/*!
    @file     voltage_divider.ino
    @author   SOSAndroid (E. Ha.)
    @license  BSD (see license.txt)

	Simple volatge divider using AD5161 over I2C bus
	A pin linked to +5V
	B pin linked to ground
	Wiper linked to analog0 pin to read back the wiper position
	
	WARNING : in this example, the read wiper position is extrapolated from 2 non precise functions : AnalogRead and Map. This drive the read output to error up to 5 steps, espacially near the extreme positions. Please keep in mind also there is a not null R value at the step 0.
	This example only checks the library as working properly
	
    @section  HISTORY

    v1.0 - First release
*/
/**************************************************************************/


#include <AnalogDevice_AD5161.h>
#include <Wire.h>
#include <SPI.h>
#include <math.h>

#define CSPIN 13
#define AD0 0
#define ANALOG_PIN 0

//AD5161 object creation
ANALOGDEVICE_AD5161 myPot(true, 0, 0, 255, 5000);

uint8_t lastposition;

void setup() {

	//Start serial
	Serial.begin(9600);
	while (!Serial) ; //wait until Serial ready
	
	Serial.println("Startup...");
	
	//Start Wire object. Unneeded here as this is done (optionnaly) by object (see lib header file - #define BUSES_INIT )
	//Wire.begin();

	//init AD5161 object
	myPot.init();
	myPot.moveTo_byStep(0, 0);
	
	Serial.print("Current stepper position: ");
	Serial.println(myPot.getPosition(), DEC);
	Serial.println("---- Ready for continuous move -----");
	Serial.println("- Value set : Value read -");
	
    //Set wiper move to position 255 by 2 steps every second
    myPot.moveTo_byTime(255, 2, 1000);
	
       
}

void loop() {

	myPot.update(); // mandatory for normal operations. This check if a move is ongoing and proceed it by steps avoiding the delay() fonction
	
	//send to serial only stepper position change
	if (lastposition != myPot.getPosition()) {
		Serial.print(myPot.getPosition(), DEC);
		Serial.print(" : ");
		Serial.println(map(analogRead(ANALOG_PIN), 0, 1023, 0, 255), DEC);
		lastposition = myPot.getPosition();
	}
	
	//start move again in opposite way if stopped
	if (myPot.getOpMode() == 0) {
		(myPot.getPosition() == 255) ? myPot.moveTo_byTime(0, 2, 1000) : myPot.moveTo_byTime(255, 2, 1000);
	}

}

/******************************************************************

Serial output example

Startup...
Current stepper position: 0
---- Ready for continuous move -----
- Value set : Value read -
2 : 3
4 : 5
6 : 7
8 : 9
10 : 11
12 : 13
14 : 16
16 : 16
18 : 18
20 : 21
22 : 23
24 : 25
26 : 27
28 : 29
30 : 31
32 : 32
34 : 34
36 : 36
38 : 39
40 : 40
42 : 43
44 : 45
46 : 47
48 : 48
50 : 50
52 : 52
54 : 54
56 : 56
58 : 59
60 : 60
62 : 63
64 : 64
66 : 66
68 : 68
70 : 70
72 : 72
74 : 74
76 : 76
78 : 79
80 : 80
82 : 82
84 : 84
86 : 86
88 : 88
90 : 90
92 : 92
94 : 94
96 : 95
98 : 97
100 : 99
102 : 101
104 : 104
106 : 106
108 : 108
110 : 110
112 : 111
114 : 113
116 : 115
118 : 117
120 : 119
122 : 121
124 : 124
126 : 126
128 : 127


*******************************************************/