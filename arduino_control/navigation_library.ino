/**
 * NAVIGATION INTERNALS FILE
 */



//Shared variables for navigation
byte state = 0;
bool interim;
short input;
float pTheta;


 
//Handles obstacle avoidance logic for the OTV
bool handleObstacleAvoidance() {
    switch(state) {
        //Back the vehicle upwards so we have some space to maneuver
        case(0): {
            setStraightPWM(-20);
            if(pingNavigation() > 25) {
                //Stop the vehicle in preparation for turning
                setStraightPWM(0);
                state++;
                break;
            }
        }
        //Turn the vehicle until there is no obstacle in sight
        case(1): {
            //Start the wobble to determine most efficient direction to turn
            setTurnPWM(20);
            state++;
            break;
        }
        //Middle of the wobble - delay for two time increments then wobble the other way and compare
        case(2): {
            input = pingNavigation();
            setTurnPWM(-20);
            state++;
            break;
        }
        case(3): {
            state++;
            break;
        }
        //Wobble reset start after measurements are taken
        case(4): {
           interim = input > pingNavigation();
           //Reset vehicle to approximately the center position
           setTurnPWM(20);
           state++;
           break;
        }
        //Wobble reset is finished, turn the OTV in the proper direction
        case(5): {
            setStraightPWM(0);
            //Start our turn in the proper direction
            setTurnPWM(interim ? 20 : -20);
            pTheta = Enes100.location.theta;
            state++;
            break;
        }
        //Turn until the OTV is properly out of the way of any obstacles
        case(6): {
            if(pingNavigation() > 50 && abs(Enes100.location.theta - pTheta) > 0.3* PI) {
                setStraightPWM(0);
                pos[0] = Enes100.location.x;
                pos[1] = Enes100.location.y;
                state++;
            }
            break;
        }
        //Drive forward and then finish obstacle avoidance logic
        case(7): {
            setStraightPWM(20);
            if (abs(Enes100.location.x - pos[0]) < 0.375 && abs(Enes100.location.y - pos[1]) < 0.375) return true;
            else {
                setStraightPWM(0);
                state = 0;
                return false;
            }
        }
    }
    return true;
}

//Handle straight-line movement to a point specified in the locations array
bool handleMovementToPoint(float destinationX, float destinationY) {
    float dX = destinationX - Enes100.location.x;
    float dY = destinationY - Enes100.location.y;
    float dA = PI * (dX < 0 ? 1 : (dY < 0 ? 2 : 0)) + atan(dY/dX);    //Calculate angle in radians
    float dI = (Enes100.location.theta < 0 ? PI * 2 : 0) + Enes100.location.theta - dA;    //Calculate offset angle in radians
    float dT = abs(dI) > PI ? dI + (PI * (dI > 0 ? -2 : 2)) : dI;    //Turn offset angle into an offset either direction angle
    //If angles do not match, turn the vehicle to match the angles, scaling turn speed with angle
    if(abs(dT) > 0.025f) {
        setTurnPWM((dT/PI)*-20 + (dT > 0 ? -10 : 10));
    } else if ((dX*dX + dY*dY) > 0.1) {
        setStraightPWM(25);
    } else {
        setStraightPWM(0);
        if(navi) {
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
