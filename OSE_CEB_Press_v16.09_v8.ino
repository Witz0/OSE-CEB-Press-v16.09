/* Open Source Ecology CEB Press v16.09 v8 Teensy Microcontroller code for Auto mode operation
  Initializes from manual position setup of pistons,
  controls MOSFET's HIGH/LOW to control two 3 position hydraulic solenoids,
  measures piston motion time relative to pressure sensor trigger,
  and repeats cycle while auto calibrating timing from previous cycles and startup positions.

  Compensates for difference in time for Extension and Contraction of Rods.
  T_extend = T_contract  * (A_cyl - A_rod) / A_cyl)

  Detects lack of soil by extreme extent of main Cylinder during compression step
  and other faults by comparing previous times to current timing.

  Faults require manual user intervention to reset to step starting position of that step or manual recalibration if power is lost to controller.

  User must manually compress brick(s) to verify correct machine function before engaging Auto Mode.

  Auto mode does a main Cyl calibration by position against a user pressed brick to calibrate for brick thickness.

  Purposefully wrote the code for novice readability? and not code efficiency
  so not enough encapsulation, math is written in longer form etc. Mmm copy pasta.
  Despite the mess it might help with any intial troubleshooting?
  Otherwise the Loop needs some OOP.

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

#define MODE_SELECT 9    //This is for the 3 position SPDT switch for Manual/OFF/Auto in docs for prior build and code shows switch auto on pin 16 and a Auto/Manual on pin 9 or pin 8 doc slide 31 and 32 differ??
#define SOLENOID_RIGHT 23
#define SOLENOID_LEFT 24
#define SOLENOID_DOWN 25
#define SOLENOID_UP 26
#define PRESSURE_SENSOR 38  //A0 F0 10 bit A2D converter
#define SWITCH_DEBOUNCE 3 //milliseconds to delay for switch debounce
#define PRESSURE_SENSOR_DEBOUNCE 20 //milliseconds to delay for pressure sensor debounce
#define COMPRESS_DELAY 500  // 1/2 sec extra to compress brick via main Cyl
#define K_A_MAIN 0.004  // T_e = T_c * (k_A)   for 1.25in x14in cylinder
#define K_A_DRAWER 0.008 // T_e = T_c * (k_A)  for 2.75in x10in cylinder
#define MAXDRIFT 5    //Sets maximum time difference in milliseconds from one cycle to the next for all steps to check for faults

// custom structures, function declarations or prototypes
bool autoMode();    //function to read if auto mode switch state is ON
bool lowPressure();    //function to read if pressure sensor is HIGH

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

  static byte cycleStep = 1;
  static bool noFaults = true;    //state of fault tracking
  static bool calibrated = false;   //sets state for first cycle calibration of main Cyl
  unsigned long previousMillis = 0;

  static unsigned long drawerRetTime = 0;   //measured
  static unsigned long drawerRetTimePre = 0;    //keep previous time of drawer Cyl Retraction Time to compare to check for  drift

  static unsigned long mainRetTime = 0;    //first reatraction measured post manual brick compression and pre auto ejection
  static unsigned long mainRetTimePre = 0;    //previous time
  static unsigned long mainCalTime = 0;     //Calculated time for post calibration return of main to user preset

  static unsigned long mainEjcTime = 0;   //time to eject brick
  static unsigned long mainEjcTimePre = 0;    //previous time

  static unsigned long mainCompTime = 0;   //measured
  static unsigned long mainCompTimePre = 0;    //keep running average of main Cyl Extension Time to compare to check for  drift

  static unsigned long drawerExtTime = 0;
  static unsigned long drawerExtTimePre = 0;   //previous time

  static unsigned long drawerMidTime = 0;    //time for retraction from removal point to mid point calculated from step 1 then measured and compared at every cycle.
  static unsigned long drawerMidTimePre = 0;    //previous time

  static float kAMain = K_A_MAIN;   //multiplier Note: if 1 isnt accurate enough for high speeds 2 or 3 could be used instead as opposed to calculus?
  static float kADrawer = K_A_DRAWER;
  unsigned long minimum = 0;    //do math
  unsigned long maximum = 0;    //and compare values
  byte drift = 0;               //for timing drift tracking

  //resets faults if user turns off auto mode
  if ((autoMode() == false) &&  noFaults == false) {
    noFaults = true;
  }

  //poll selector switch continuously and check for fault condition at start of every cycle
  while (autoMode() == true && noFaults == true) {

    switch (cycleStep) {

      //Step 1 Retraction drawer Cyl RIGHT measure T_ret at Presure sensor high or calibrate main retraction if first cycle or faults
      case 1:
        {
          //Run first cycle calibration main retraction if first cycle or faults
          if (calibrated == false) {
            while ((lowPressure() == true) && (autoMode() == true)) {
              previousMillis = millis();
              digitalWrite(SOLENOID_DOWN, HIGH);
            }
            digitalWrite(SOLENOID_DOWN, LOW);
            mainRetTime = millis() - previousMillis;
            mainRetTimePre = mainRetTime;
            calibrated = true;
          }

          //return main cylinder to user set point
          while ((lowPressure() == true) && (autoMode() == true)) {
            mainCalTime = mainRetTime * kAMain;
            previousMillis = millis();
            while ((millis() - previousMillis) < mainCalTime) {
              digitalWrite(SOLENOID_UP, HIGH);
            }
            digitalWrite(SOLENOID_UP, LOW);

            //Retraction drawer Cyl RIGHT measure T_ret at Presure sensor high
            while ((lowPressure() == true) && (autoMode() == true)) {
              previousMillis = millis();
              digitalWrite(SOLENOID_RIGHT, HIGH);
            }
            digitalWrite(SOLENOID_RIGHT, LOW);
            drawerRetTime = millis() - previousMillis;

            if (drawerRetTimePre == 0) {
              drawerRetTimePre = drawerRetTime;
            }
            else {
              if (drawerRetTime != drawerRetTimePre) {
                minimum = min(drawerRetTime, drawerRetTimePre);
                maximum = max(drawerRetTime, drawerRetTimePre);
                drift = maximum - minimum;
                if (drift > MAXDRIFT) {
                  noFaults = false;
                  calibrated = false;
                  cycleStep = 1;
                  break;
                }
              }
            }
          }
          drawerRetTimePre = drawerRetTime;
        }

      //Step 2 Ejection by extending main cyl UP until pressure sensor high measure T_ext
      case 2:
        {
          while ((lowPressure() == true) && (autoMode() == true)) {
            previousMillis = millis();
            digitalWrite(SOLENOID_UP, HIGH);
          }
          digitalWrite(SOLENOID_UP, LOW);
          mainEjcTime = millis() - previousMillis;

          if (mainEjcTimePre == 0) {
            mainEjcTimePre = mainEjcTime;
          }
          else {
            if (mainEjcTime != mainEjcTimePre) {
              minimum = min(mainEjcTime, mainEjcTimePre);
              maximum = max(mainEjcTime, mainEjcTime);
              drift = maximum - minimum;
              if (drift > MAXDRIFT) {
                noFaults = false;
                calibrated = false;
                cycleStep = 2;
                break;
              }
            }
          }
          mainEjcTimePre = mainEjcTime;
        }

      //Step 3 Brick Removal 2nd Cyl extended LEFT until Presure sensor high
      case 3:
        {
          while ((lowPressure() == true) && (autoMode() == true)) {
            previousMillis = millis();
            digitalWrite(SOLENOID_LEFT, HIGH);
          }
          digitalWrite(SOLENOID_LEFT, LOW);
          drawerExtTime = millis() - previousMillis;

          if (drawerExtTimePre == 0) {
            drawerExtTimePre = drawerExtTime;
          }
          else {
            if (drawerExtTime != drawerExtTimePre) {
              minimum = min(drawerExtTime, drawerExtTimePre);
              maximum = max(drawerExtTime, drawerExtTime);
              drift = maximum - minimum;
              if (drift > MAXDRIFT) {
                noFaults = false;
                cycleStep = 3;
                break;
              }
            }
          }
          drawerExtTimePre = drawerExtTime;
        }


      //Step 4 Soil Load main Cyl moves DOWN/retracts and soil enters chamber
      case 4:
        {
          while ((lowPressure() == true) && (autoMode() == true)) {
            previousMillis = millis();
            while ((millis() - previousMillis) <= mainRetTime) {
              digitalWrite(SOLENOID_DOWN, HIGH);
            }
            digitalWrite(SOLENOID_DOWN, LOW);
            mainRetTime = millis() - previousMillis;
          }

          if (mainRetTimePre == 0) {
            mainRetTimePre = mainRetTime;
          }
          else {
            if (mainRetTime != mainRetTimePre) {
              minimum = min(mainRetTime, mainRetTimePre);
              maximum = max(mainRetTime, mainRetTime);
              drift = maximum - minimum;
              if (drift > MAXDRIFT) {
                noFaults = false;
                cycleStep = 4;
                break;
              }
            }
          }
          mainRetTimePre = mainRetTime;
        }

      //Step 5 Chamber/Drawer Closure drawer retraction time to midpoint is calculated from initial full contraction from the midpoint (step 1 measurement)
      case 5:
        {
          while ((lowPressure() == true) && (autoMode() == true)) {
            drawerMidTime = drawerExtTime / kADrawer ;
            previousMillis = millis();
            while ((millis() - previousMillis) <= drawerMidTime) {
              digitalWrite(SOLENOID_RIGHT, HIGH);
            }
            digitalWrite(SOLENOID_RIGHT, LOW);
          }
          if ( drawerMidTimePre == 0) {
            drawerMidTimePre =  drawerMidTime;
          }
          else {
            if ( drawerMidTime !=  drawerMidTimePre) {
              minimum = min( drawerMidTime,  drawerMidTimePre);
              maximum = max( drawerMidTime,  drawerMidTime);
              drift = maximum - minimum;
              if (drift > MAXDRIFT) {
                noFaults = false;
                cycleStep = 5;
                break;
              }
            }
          }
          drawerMidTimePre =  drawerMidTime;
        }

      //Step 6 Brick Pressing Main Cyl moves to T_ext + 1/2 sec compression delay
      case 6:
        {
          while ((lowPressure() == true) && (autoMode() == true)) {
            previousMillis = millis();
            digitalWrite(SOLENOID_UP, HIGH);
          }
          previousMillis = millis() - previousMillis;
          mainCompTime = previousMillis;
          delay(COMPRESS_DELAY);
          digitalWrite(SOLENOID_UP, LOW);

          if ( mainCompTimePre == 0) {
            mainCompTimePre =  mainCompTime;
          }
          else {
            if ( mainCompTime !=  mainCompTimePre) {
              minimum = min( mainCompTime,  mainCompTimePre);
              maximum = max( mainCompTime,  mainCompTime);
              drift = maximum - minimum;
              if (drift > MAXDRIFT) {
                noFaults = false;
                cycleStep = 6;
                break;
              }
            }
          }
          mainCompTimePre =  mainCompTime;
          cycleStep = 1;
        }

    }
  }
}
//end of main loop

//custom functions

//reads mode switch state HIGH/true is auto mode ON and LOW/false is AUTO mode OFF PAUSE or MANUAL due to 3 position switch
bool autoMode() {
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

//reads pressure sensor state HIGH is false and LOW is true
bool lowPressure() {
  if (digitalRead(PRESSURE_SENSOR) == HIGH) {
    delay(PRESSURE_SENSOR_DEBOUNCE);
    if (digitalRead(PRESSURE_SENSOR) == HIGH) {
      return false;
    }
    else {
      return true;
    }
  }
  else {
    return true;
  }
}



