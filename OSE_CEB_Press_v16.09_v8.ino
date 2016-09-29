/* Open Source Ecology CEB Press v16.09 v8 Teensy Microcontroller code for Auto mode operation
Initializes from manual position setup of pistons,
pulls MOSFET's high and low to control two 3 position hydraulic solenoids,
measures piston motion time relative to pressure sensor trigger,
and repeats cycle while auto calibrating timing from previous cycles and startup positions.
Compensates for difference in time for Extension and Contraction of Rods.
T_extend = T_contract  * (A_cyl - A_rod) / A_cyl
Detects lack of soil by extreme extent of main Cylinder

Contributions by:
Abe Anderson


License:
See GPLv3 license file in repo.

*/


//Heavy use of defines may make it easier for non coders to make adjustments for troubleshooting and custom changes

#define MODE_SELECT 9
#define SOLENOID_RIGHT 23
#define SOLENOID_LEFT 24	//PWM capable
#define SOLENOID_DOWN 25	//PWM
#define SOLENOID_UP 26		//PWM
#define PRESSURE_SENSOR 38  //A0

#define COMPRESS_DELAY 500	// 1/2 sec extra to compress brick via main Cyl
//#define DRIFT_ADJUST ?
//#define CYLA ?
//#define RODA ?


void setup() {
	
 //constants for math?
 
 //initialize pin I/O
 pinMode(MODE_SELECT, INPUT);
 pinMode(SOLENOID_RIGHT, OUTPUT);
 pinMode(SOLENOID_LEFT, OUTPUT);
 pinMode(SOLENOID_DOWN, OUTPUT);
 pinMode(SOLENOID_UP, OUTPUT);
 pinMode(PRESSURE_SENSOR, INPUT);
 
 //setup interrupt to mode selector
 
 //volatiles for possible pause/intterupt
 
 

}

void loop() {
/*
Auto mode starting assumptions
Earth has been  loaded into chamber in manual setup?
no pressure on lines
Main Cylinder is set to apropriate position for block thichness
Secondary is correctly centered in middle position
*/


}
