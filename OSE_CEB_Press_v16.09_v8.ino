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

//includes 		*need to make sure correct Teensy libs get included if any are needed
//Teensyduino add-on software v1.30 is compatible with arduino IDE 1.6.11
//#include "Aurduino.h"

//Heavy use of defines may make it easier for non coders to make adjustments for troubleshooting and custom changes

#define MODE_SELECT 9	
#define SOLENOID_RIGHT 23	
#define SOLENOID_LEFT 24  
#define SOLENOID_DOWN 25  
#define SOLENOID_UP 26    
#define PRESSURE_SENSOR 38  //A0 F0 10 bit A2D converter
#define SWITCH_DEBOUNCE 3	//milliseconds to delay for switch debounce
#define PRESSURE_SENSOR_DEBOUNCE 20	//milliseconds to delay for pressure sensor debounce
#define COMPRESS_DELAY 500  // 1/2 sec extra to compress brick via main Cyl
#define K_A 0.001	// T_e = T_c * (k_A)		*need to find correct value assuming this methods make sense*


// custom structures functions declarations and prototypes
bool readMode();
bool readPressure();

void setup() {
  
 //volatiles and constants for math?
 
 
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
 

}

void loop() {
/*
Auto mode starting assumptions
system is empty of earth and ready to measure motion time
no pressure on lines
Main Cylinder is set to apropriate position for block thichness
Secondary is correctly centered in middle position

*/
unsigned long previousMillis = 0;
static unsigned long mainRetTime;
static unsigned long drawerRetTime;
static unsigned long mainExtTime;
static unsigned long drawerExtTime;
static float areaFraction = K_A;

//poll selector switch continuously
while(readMode()==LOW) {

	//Step 1 Retraction measure T_ret at Presure sensor high
	while(readPressure()==LOW){
		previousMillis = millis();
		digitalWrite(SOLENOID_LEFT, HIGH);
	}
	drawerRetTime = millis() - previousMillis;

	//Step 2 Ejection by extending main cyl until pressure sensor high measure T_ext
	
	
	//Step 3 Brick Removal 2ndCyl extended until Presure sensor high

	//Step 4 Soil Load main Cyl moves down and soil enters chamber measure T_ret

	//Step 5 Chamber/Drawer Closure T_mid is calculated from T_ret

	//Step 6 Brick Pressing Main Cyl moves to T_ext + 1/2 sec compression delay

 }
}

//end of main
//custom functions

bool readMode(){
	if(digitalRead(MODE_SELECT)==HIGH){
		delay(SWITCH_DEBOUNCE);
		if(digitalRead(MODE_SELECT)==HIGH){
			return true;
			}
	}
	
	else {
		return false;
	}
}

bool readPressure(){
	if(digitalRead(PRESSURE_SENSOR)==HIGH){
		delay(PRESSURE_SENSOR_DEBOUNCE);
		if(digitalRead(PRESSURE_SENSOR)==HIGH){
			return true;
			}
	}
	
	else {
		return false;
	}
}


