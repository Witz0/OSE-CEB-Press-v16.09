/* Open Source Ecology CEB Press v16.09 v8 Teensy Microcontroller code for Auto mode operation
  Initializes from manual position setup of pistons,
  controls MOSFET's HIGH/LOW to control two 3 position hydraulic solenoids,
  measures piston motion time relative to pressure sensor trigger,
  and repeats cycle while auto calibrating timing from previous cycles and startup positions.
  Compensates for difference in time for Extension and Contraction of Rods.
  T_extend = T_contract  * (A_cyl - A_rod) / A_cyl
  Detects lack of soil by extreme extent of main Cylinder
  Faults require manual user reset to proper starting position.
  User must manually compress brick(s) to verify correct machine function before engaging Auto Mode.

  Contributions by:
  Abe Anderson


  License:
  See GPLv3 license file in repo.
  Hmm thought this was best copyleft, but previous code was CC-BY-SA need to double check

*/

//includes     *need to make sure correct Teensy libs get included if any are needed
//Teensyduino add-on software v1.30 is compatible with arduino IDE 1.6.11
//#include "Aurduino.h"

//Heavy use of defines may make it easier for non coders to make adjustments for troubleshooting and custom changes

#define MODE_SELECT 16    //in docs for prior build and code shows switch auto on pin 16 and a Auto/Manual on pin 9 ??
#define SOLENOID_RIGHT 23
#define SOLENOID_LEFT 24
#define SOLENOID_DOWN 25
#define SOLENOID_UP 26
#define PRESSURE_SENSOR 38  //A0 F0 10 bit A2D converter
#define SWITCH_DEBOUNCE 3 //milliseconds to delay for switch debounce
#define PRESSURE_SENSOR_DEBOUNCE 20 //milliseconds to delay for pressure sensor debounce
#define COMPRESS_DELAY 500  // 1/2 sec extra to compress brick via main Cyl
#define K_A_MAIN 0.001  // T_e = T_c * (k_A)    *need to find correct value assuming this method makes sense*
#define K_A_DRAWER 0.000888 // T_e = T_c * (k_A)    *calculated from [(1.126-1.25)/1.126]

// custom structures, function declarations or prototypes
bool readMode();    //function to read if auto mode switch state is ON
bool readPressure();    //function to read if pressure sensor is HIGH

void setup() {

  //initialize pin I/O Inputs use internal resistor pullups where needed and outputs get set low to prevent glitches while booting
  pinMode(MODE_SELECT, INPUT);
  digitalWrite(MODE_SELECT, INPUT_PULLUP);
  pinMode(SOLENOID_RIGHT, OUTPUT);
  digitalWrite(SOLENOID_RIGHT, LOW);
  pinMode(SOLENOID_LEFT, OUTPUT);
  digitalWrite(SOLENOID_LEFT, LOW);
  pinMode(SOLENOID_DOWN, OUTPUT);
  digitalWrite(SOLENOID_DOWN, LOW);
  pinMode(SOLENOID_UP, OUTPUT);
  digitalWrite(SOLENOID_UP, LOW);
  pinMode(PRESSURE_SENSOR, INPUT);
  digitalWrite(PRESSURE_SENSOR, INPUT_PULLUP);

}

void loop() {
  /*
    Auto mode starting assumptions
    User has manually pressed a brick to test system for proper function
    and it is ready to retract the drawer, eject, and remove the brick.
    This allows user to set brick thickness
    and the auto mode to calculate motion times from starting positions.
  */
  unsigned long previousMillis = 0;
  //unsigned long currentMillis = 0;
  static unsigned long drawerRetTime;   //measured
  static unsigned long drawerRetTimeAvg;    //keep running average of drawer Cyl Retraction Time to compare to check for  drift
  static unsigned long mainExtTime;   //measured
  static unsigned long mainExtTimeAvg;    //keep running average of main Cyl Extension Time to compare to check for  drift
  unsigned long drawerExtTime;    //calculated from drawer retraction from middle start point
  unsigned long mainRetTime;    //calculated from main ejection extension time from start point
  unsigned long drawerMidTime;    //calculated from drawer retraction from middle start point
  unsigned long mainMidTime;    //calculated from main ejection extension time from start point
  static float kAMain = K_A_MAIN;   //multiplier Note: if 1 isnt accurate enough 2 or 3 could be used instead as opposed to calculus
  static float kADrawer = K_A_DRAWER;

  //poll selector switch continuously
  while (readMode() == true) {

    //Step 1 Retraction 2nd Cyl RIGHT measure T_ret at Presure sensor high
    while ((readPressure() == false) && (readMode() == true)) {
      previousMillis = millis();
      digitalWrite(SOLENOID_RIGHT, HIGH);
    }
    drawerRetTime = millis() - previousMillis;
    digitalWrite(SOLENOID_RIGHT, LOW);


    //Step 2 Ejection by extending main cyl UP until pressure sensor high measure T_ext
    while ((readPressure() == false) && (readMode() == true)) {
      previousMillis = millis();
      digitalWrite(SOLENOID_UP, HIGH);
    }
    mainExtTime = millis() - previousMillis;
    digitalWrite(SOLENOID_UP, LOW);
    //need to bump main cyl foot down for clearance/pressure release
    //digitalWrite(SOLENOID_DOWN, HIGH);
    //delay(100)
    //digitalWrite(SOLENOID_DOWN, LOW);

    //Step 3 Brick Removal 2nd Cyl extended LEFT until Presure sensor high

    while ((readPressure() == false) && (readMode() == true)) {
      previousMillis = millis();
      digitalWrite(SOLENOID_LEFT, HIGH);
    }
    drawerRetTime = millis() - previousMillis;
    digitalWrite(SOLENOID_LEFT, LOW);

    //Step 4 Soil Load main Cyl moves DOWN/retracts and soil enters chamber

    while ((readPressure() == false) && (readMode() == true)) {
      mainRetTime = mainExtTime / kAMain;   //type issues for math may need seperate function it could also check averages.
      previousMillis = millis();
      while ((millis() - previousMillis) <= mainRetTime) {
        digitalWrite(SOLENOID_DOWN, HIGH);
      }
      drawerRetTime = millis() - previousMillis;
      digitalWrite(SOLENOID_DOWN, LOW);
    }
    //Step 5 Chamber/Drawer Closure T_mid is calculated from T_ret

    //Step 6 Brick Pressing Main Cyl moves to T_ext + 1/2 sec compression delay

  }
}

//end of main
//custom functions

//reads mode switch state HIGH is auto mode ON and LOW is AUTO mode OFF PAUSE or MANUAL due to 3 position switch
bool readMode() {
  if (digitalRead(MODE_SELECT) == HIGH) {
    delay(SWITCH_DEBOUNCE);
    if (digitalRead(MODE_SELECT) == HIGH) {
      return true;
    }
    else {
      return false;
    }
  }
  else {
    return false;
  }
}

//reads pressure sensor state
bool readPressure() {
  if (digitalRead(PRESSURE_SENSOR) == HIGH) {
    delay(PRESSURE_SENSOR_DEBOUNCE);
    if (digitalRead(PRESSURE_SENSOR) == HIGH) {
      return true;
    }
    else {
      return false;
    }
  }
  else {
    return false;
  }
}













