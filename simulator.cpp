#include "Enes100.h"
#include "Tank.h"
#include <math.h>

//Mission details
int time = 0;
//Navigation requirements
bool obstacle = false;
bool stationary = false;
float destination[4];
float pos[2];
//Other required information
bool watchdog;
bool prevWatchdog;
short check = 0;

//Shared variables for navigation
short state = 0;
bool interim;
float input;
float pTheta;

bool rForward;
bool lForward;

float p20X;
float p20Y;
float p20Theta;

bool navi = false;




/* This is an example function to make both motors drive
 * at the given power.
 */
void setStraightPWM(int speed) {
    Tank.setLeftMotorPWM(speed);
    Tank.setRightMotorPWM(speed);
    rForward = lForward = speed > 0;
}

void setTurnPWM(int speed) {
    Tank.setLeftMotorPWM(-speed);
    Tank.setRightMotorPWM(speed);
    rForward = speed > 0;
    lForward = !rForward;
}

const float PI = 3.1415926535;

 
//Handles obstacle avoidance logic for the OTV
bool handleObstacleAvoidance() {
    switch (state) {
        //Back the vehicle upwards so we have some space to maneuver
        case (0): {
            setStraightPWM(-50);
            if (Tank.readDistanceSensor(1) > 0.25) {
                //Stop the vehicle in preparation for turning
                setStraightPWM(0);
                state++;
            }
            break;
        }
        //Turn the vehicle until there is no obstacle in sight
        case (1): {
            //Start the wobble to determine most efficient direction to turn
            setTurnPWM(50);
            state++;
            break;
        }
        //Middle of the wobble - delay for two time increments then wobble the other way and compare
        case (2): {
            input = Tank.readDistanceSensor(1);
            setTurnPWM(-50);
            state++;
            break;
        }
        case (3): {
            state++;
            break;
        }
        //Wobble reset start after measurements are taken
        case (4): {
            interim = input > Tank.readDistanceSensor(1);
            //Reset vehicle to approximately the center position
            setTurnPWM(50);
            state++;
            break;
        }
        //Wobble reset is finished, turn the OTV in the proper direction
        case (5): {
            setStraightPWM(0);
            //Start our turn in the proper direction
            setTurnPWM(interim ? 50 : -50);
            state++;
            pTheta = Enes100.location.theta;
            break;
        }
        //Turn until the OTV is properly out of the way of any obstacles
        case (6): {

            if (Tank.readDistanceSensor(1) > 0.5 && abs(Enes100.location.theta - pTheta) > 0.3 * PI) {
                setStraightPWM(0);
                pos[0] = Enes100.location.x;
                pos[1] = Enes100.location.y;
                state++;
            }

            break;
        }
        //Drive forward and then finish obstacle avoidance logic
        case (7): {
            setStraightPWM(50);
            if (abs(Enes100.location.x - pos[0]) < 0.375 && abs(Enes100.location.y - pos[1]) < 0.375) {
                return true;
            } else {
                setStraightPWM(0);
                state = 0;
                return false;
            }
            break;
        }
    }
    return true;
}

//Handle straight-line movement to a point specified in the locations array
bool handleMovementToPoint(float destinationX, float destinationY) {
    float dX = destinationX - Enes100.location.x;
    float dY = destinationY - Enes100.location.y;
    float dA = PI * (dX < 0 ? 1 : (dY < 0 ? 2 : 0)) + atan(dY / dX);        //Calculate angle in radians
    float dI = (Enes100.location.theta < 0 ? PI * 2 : 0) + Enes100.location.theta -dA;        //Calculate offset angle in radians
    float dT = abs(dI) > PI ? dI + (PI * (dI > 0 ? -2 : 2)) : dI;        //Turn offset angle into an offset either direction angle
    //If angles do not match, turn the vehicle to match the angles, scaling turn speed with angle
    if (abs(dT) > 0.025f) {
        setTurnPWM((dT / PI) * -200 + (dT > 0 ? -40 : 40));
    } else if ((dX * dX + dY * dY) > 0.1) {
        setStraightPWM(120);
    } else {
        setStraightPWM(0);

        if (navi) {
            navi = false;
            destination[0] = destination[2];
            destination[1] = destination[3];
            destination[2] = 0;
            destination[3] = 0;
        }

        return true;
    }

    return false;
}

bool navigationWatchdog() {
    if (time % 20 == 0) {
        //Update OTV location
        Enes100.updateLocation();
        //Check to see if the no-movement watchdog should trigger and run said subroutine
        if (rForward == lForward) {
            if (abs(p20X - Enes100.location.x) < 0.00125 && abs(p20Y - Enes100.location.y) < 0.00125) {
                return true;
            }
        } else {
            if (abs(p20Theta - Enes100.location.theta) < 0.00125) {
                return true;
            }
        }
        //Update previous location variables every 20 ms
        p20X = Enes100.location.x;
        p20Y = Enes100.location.y;
        p20Theta = Enes100.location.theta;
    }
    //We should do a passthrough on the watchdog variable to let possible other things continue their work
    return watchdog;
}

float watchdogX;
float watchdogY;
float watchdogTheta;

//Handles OTV is stuck movement for the OTV, getting it out of pickles
bool handleWatchdogMovement() {
    switch (check) {
        //Flip input booleans to make sure we go the opposite of the way that got us into this mess
        case (0) : {
            rForward = !rForward;
            lForward = !lForward;

            if (rForward != lForward) {
                setTurnPWM(rForward ? 20 : -20);
            } else {
                setStraightPWM(rForward ? 20 : -20);
            }

            //Set booleans for later comparison
            watchdogX = Enes100.location.x;
            watchdogY = Enes100.location.y;
            watchdogTheta = Enes100.location.theta;
            navi = true;

            break;
        }
        case (1) : {
            //Check which type of motion we should do, and turn/move back exactly some multiple of the watchdog check's worth
            if (rForward != lForward) {
                if (abs(watchdogTheta - Enes100.location.theta) < 0.25) {
                    return true;
                }
            } else {
                if (abs(watchdogX - Enes100.location.x) < 0.25 && abs(watchdogY - Enes100.location.y) < 0.25) {
                    return true;
                }
            }
            setStraightPWM(0);
            break;
        }
        //Then we redo the arrays and set the destination to be somewhere out of the way of the OTV
        case (2): {
            //Move up current destination coordinates
            destination[2] = destination[0];
            destination[3] = destination[1];
            //Recalculate new destination coordinates from old ones
            destination[0] = Enes100.location.x +cos((Enes100.location.theta < 0 ? PI * 2 : 0) + Enes100.location.theta + Enes100.location.theta > 0 ? PI / 2 : -PI / 2) * 0.7;
            destination[1] = Enes100.location.y + sin((Enes100.location.theta < 0 ? PI * 2 : 0) + Enes100.location.theta + Enes100.location.theta > 0 ? PI / 2 : -PI / 2) * 0.7;
        }
        case (3) : {
            return false;
        }
    }
    check++;
    return true;
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
        if (Tank.readDistanceSensor(1) > 0.10 && !obstacle) {
            return handleMovementToPoint(destination[0], destination[1]);
        } else {
            obstacle = handleObstacleAvoidance();
        }
    }

    //Return false because we haven't gotten here otherwise
    return false;
}

/* The code inside void setup() runs only once, before the code in void loop(). */
void setup() {
    //Establish connection to WIFI module & VISION system
    Tank.begin();
    while (!Enes100.begin("Otterworldly OTV", WATER, 3, 8, 9));

    //Update location and required objectives
    Enes100.updateLocation();
    destination[0] = Enes100.missionSite.x;
    destination[1] = Enes100.missionSite.y;
}

/* The code in void loop() runs repeatedly forever */
void loop() {
    time++;
    //Here's where we switch beween handling navigation and mission logic
    if (stationary) {
        if (true) {
            stationary = false;
            destination[0] = 3.5;
            destination[1] = 1.5;
        }
    } else {
        if (runNavigation()) stationary = true;
    }
    prevWatchdog = watchdog;
    watchdog = !stationary && navigationWatchdog();
    if (prevWatchdog != watchdog && !watchdog) {
        destination[0] = destination[2];
        destination[1] = destination[3];
    }
    delay(1);
}
