#include <math.h>
#include "Enes100.h"



/**
 * CONSTANT GLOBAL VARIABLE SPECIFICATION SECTION
 */
 
//Analog pin specifications
const short pINPUT_NAV = 14;
const short pINPUT_DEPTH = 15;
const short pINPUT_CONDUCTIVITY = 18;
const short pINPUT_COLOR = 19;
//Digial pin specifications
const short pWIFI_OUT = 13;
const short pWIFI_IN = 12;
const short pLEFT_FORWARD = 11;
const short pLEFT_BACKWARD = 10;
const short pCOLOR_S1 = 9;
const short pCOLOR_S2 = 8;
const short pRIGHT_FORWARD = 5;
const short pRIGHT_BACKWARD = 6;
const short pTRIGGER_DEPTH = 4;
const short pPUMP_PWM = 3;
const short pTRIGGER_NAV = 2;

//Mission-specific constants
const short ARUCO = 465;
const short PUMP_TIME = 20000;
const short SENSOR_HEIGHT = 101;
const short BASE_HEIGHT = 9;
const short CONDUCTIVITY_BASELINE = 0;

/**
 * NON-CONSTANT GLOBAL VARIABLE SPECIFICATION SECTION
 */

//Mission details
int time = 0;
//Navigation requirements
bool obstacle = false;
bool reconnect = false;
bool stationary = false;
float destination[4];
float pos[2];
//Other required information
byte function = 0;
int timing;
bool polluted;
//PWM communication information
bool rBackwards = false;
bool lBackwards = false;
//Watchdog information
bool watchdog;
bool prevWatchdog;
bool navi;
byte check = 0;



/**
 * MAIN FUNCTION SECTION - DO NOT TOUCH WITHOUT CONSULTING ALEX
 */
void setup() {
    //Analog pins to input
    pinMode(pINPUT_NAV, INPUT);
    pinMode(pINPUT_DEPTH, INPUT);
    pinMode(pINPUT_CONDUCTIVITY, INPUT);
    pinMode(pINPUT_COLOR, INPUT);
    //Digital pins to output/input
    pinMode(pWIFI_OUT, OUTPUT);
    pinMode(pWIFI_IN, INPUT);
    pinMode(pLEFT_FORWARD, OUTPUT);
    pinMode(pLEFT_BACKWARD, OUTPUT);
    pinMode(pCOLOR_S1, OUTPUT);
    pinMode(pCOLOR_S2, OUTPUT);
    pinMode(pRIGHT_FORWARD, OUTPUT);
    pinMode(pRIGHT_BACKWARD, OUTPUT);
    pinMode(pTRIGGER_DEPTH, OUTPUT);
    pinMode(pPUMP_PWM, OUTPUT);
    pinMode(pTRIGGER_NAV, OUTPUT);
    
    //Establish connection to WIFI module & VISION system
    while(!Enes100.begin("Otterworldly OTV", WATER, ARUCO, pWIFI_OUT, pWIFI_IN));
    Enes100.println("Otterworldly OTV beginning mission");
    
    //Update location and required objectives
    Enes100.updateLocation();
    destination[0] = Enes100.missionSite.x;
    destination[1] = Enes100.missionSite.y;
}

void loop() {
    //Increment a time value used for controlling the PWM of the motors and pump
    time++;
    //Make sure we stay connected to the VISION system and reconnect if we drop connection
    while(!Enes100.ping()){
        Enes100.begin("Otterworldly OTV", WATER, ARUCO, pWIFI_OUT, pWIFI_IN);
        reconnect = true;
    }
    //Here's where we switch beween handling navigation and mission logic
    if (stationary)
        if (runMissionLogic()){
            stationary = false;
            destination[0] = 3.5;
            destination[1] = 1.5;
        }
    else if (runNavigation()) stationary = true;
    //We handle navigation PWM right before the end to make sure any decisions for PWM don't have to wait another ms to be put into action
    if (!stationary) handlePWM();
    //Here we handle the watchdog code to make sure that we don't break our motors running into the wall
    prevWatchdog = watchdog;
    watchdog = stationary ? false : navigationWatchdog();
    if(prevWatchdog != watchdog && !watchdog) {
        destination[0] = destination[2];
        destination[1] = destination[3];
    }
    //We delay once every loop to make sure we run each about loop once per millisecond and not more, to allow for the PWM subsystem to properly do its work
    delay(1);
}


 
//Handles any and all mission sensor logic. May require other subfunctions to be created for its use. Returns true ONLY when its task at this location is completed
bool runMissionLogic() {
    switch(function) {
        case(0): Enes100.mission(DEPTH, pingDepth());    //Read depth sensor & post final depth value to mission info
        case(1): digitalWrite(pPUMP_PWM, HIGH);    //Enable the pump, then wait for 2000ms (2s) to collect water
        case(2): {
            if (time >= PUMP_TIME + timing) function++;
            return false;
        }
        case(3): digitalWrite(pPUMP_PWM, LOW);    //Turn the pump off
        case(4): polluted = pingGreen() > (2*pingRed() + pingBlue());    //Read color sensor for mission values
        case(5): {
            if (analogRead(pINPUT_CONDUCTIVITY) > 400) {    //If we have the sensor in the water, test the conductivity
                switch(analogRead(pINPUT_CONDUCTIVITY) < CONDUCTIVITY_BASELINE) {
                    case(true):  Enes100.mission(WATER_TYPE, polluted ? FRESH_POLLUTED : FRESH_UNPOLLUTED);    //Report the values to the mission
                    case(false): Enes100.mission(WATER_TYPE, polluted ? SALT_POLLUTED : SALT_UNPOLLUTED);
                }
                function++;
            }
            return false;
        }
        case(6): return true;
    }
    //When at the end of the function, move one stage forwards in the mission collection setup unless specified otherwise
    function++;
    return false;
}


 
//Handles any and all nagivation tasks. This is the brains behind the movement system, and will do the most interfacing with the VISION system due to position updates. Returns true ONLY when objective position has been reached
bool runNavigation() {
    //Update OTV location for next calculations
    Enes100.updateLocation();

    //If the watchdog is triggered, it takes priority over all other forms of movement
    if (watchdog) {
        watchdog = handleWatchdogMovement();
    } else {
        //Use ultrasonic sensor input for obstacle avoidance if required
        if (pingNavigation() > 10 && !obstacle) return handleMovementToPoint(destination[0], destination[1]);
        else obstacle = handleObstacleAvoidance();
    }
    
    //Return false because we haven't gotten here otherwise
    return false;
}
