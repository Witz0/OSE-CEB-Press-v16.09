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
Hmm thought this was best copyleft, but previous code was CC-BY-SA need to double check

*/

//includes make sure correct Teensy libs get included if any are needed
//Teensyduino add-on software v1.30 is compatible with arduino IDE 1.6.11
//#include "Aurduino.h"

//Heavy use of defines may make it easier for non coders to make adjustments for troubleshooting and custom changes

#define MODE_SELECT 9
#define SOLENOID_RIGHT 23
#define SOLENOID_LEFT 24	//PWM capable pins
#define SOLENOID_DOWN 25	//PWM
#define SOLENOID_UP 26		//PWM
#define PRESSURE_SENSOR 38  //A0 F0 10 bit A2D converter

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
 
 //Enable internal Pullup resistors on inputs for reading sensors
 digitalWrite(MODE_SELECT, INPUT_PULLUP);
 digitalWrite(PRESSURE_SENSOR, INPUT_PULLUP);
 
 //setup interrupt to mode selector
 
 //volatiles for possible pause/intterupt
 
 

}

void loop() {
/*
Auto mode starting assumptions
system is empty of earth and ready to measure motion time
no pressure on lines
Main Cylinder is set to apropriate position for block thichness
Secondary is correctly centered in middle position

*/
//Step 1 Retraction measure T_ret until Presure sensor high

//Step 2 Ejection by extending main cyl until pressure sensor high measure T_ext

//Step 3 Brick Removal 2ndCyl extended until PPresure sensor high

//Step 4 Soil Load main Cyl moves down and soil enters chamber measure T_ret

//Step 5 Chamber/Drawer Closure T_mid is calculated from T_ret

//Step 6 Brick Pressing Main Cyl moves to T_ext + 1/2 sec compression delay

}
