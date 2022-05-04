/**
 * MOVEMENT WATCHDOG SYSTEM FILE
 */



//Variables for watchdog sensing
float p20X;
float p20Y;
float p20Theta;
int lastUpdate;
//Variables for watchdog movement
float watchdogX;
float watchdogY;
float watchdogTheta;



//Handles checks for if we need to engage movement because the OTV is stuck
bool navigationWatchdog() {
    if (time % 20 == 0) {
        //Update OTV location
        Enes100.updateLocation();
        //Check to see if the no-movement watchdog should trigger and run said subroutine
        if (rBackwards == lBackwards) {
            if (abs(p20X - Enes100.location.x) < 0.00125 && abs(p20Y - Enes100.location.y) < 0.00125) return true;
        } else {
            if (abs(p20Theta - Enes100.location.theta) < 0.00125) return true;
        }
        //Update previous location variables every 20 ms
        p20X = Enes100.location.x;
        p20Y = Enes100.location.y;
        p20Theta = Enes100.location.theta;
    }
  //We should do a passthrough on the watchdog variable to let possible other things continue their work
  return watchdog;
}

//Handles OTV is stuck movement for the OTV, getting it out of pickles
bool handleWatchdogMovement() {
    switch(check) {
        //Flip input booleans to make sure we go the opposite of the way that got us into this mess
        case(0): {
            rBackwards = !rBackwards;
            lBackwards = !lBackwards;
      
            //Set variables for later comparison
            watchdogX = Enes100.location.x;
            watchdogY = Enes100.location.y;
            watchdogTheta = Enes100.location.theta;
            navi = true;
            break;
        }
        case(1): {
            //Check which type of motion we should do, and turn/move back exactly some multiple of the watchdog check's worth
            if (rBackwards != lBackwards) {
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
        case(2): {
            //Move up current destination coordinates
            destination[2] = destination[0];
            destination[3] = destination[1];
            //Recalculate new destination coordinates from old ones
            destination[0] = Enes100.location.x + cos((Enes100.location.theta < 0 ? PI * 2 : 0) + Enes100.location.theta + Enes100.location.theta > 0 ? PI/2 : -PI/2)*0.7;
            destination[1] = Enes100.location.y + sin((Enes100.location.theta < 0 ? PI * 2 : 0) + Enes100.location.theta + Enes100.location.theta > 0 ? PI/2 : -PI/2)*0.7;
        }
        case(3): {
            return false;
        }
    }
    check++;
    return true;
}
