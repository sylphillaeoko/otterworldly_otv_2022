/**
 * PWM INTERFACE LIBRARY FILE
 */


//Global variables for movement intercommunication - DO NOT SET, use functions to interface with
byte rPWM = 0;
byte lPWM = 0;
bool rBackwardsPrevious = false;
bool lBackwardsPrevious = false;



//Sets wheel speed from -100 to 100
//We use a secondary variable here to store for use in PWM switching later
void setWheelSpeed(bool right, byte speed) {
    if (right) {
        rBackwardsPrevious = rBackwards;
        rBackwards = speed > 0 ? false : true;
        rPWM = abs(speed);
    } else {
        lBackwardsPrevious = lBackwards;
        lBackwards = speed > 0 ? false : true;
        lPWM = abs(speed);
    }
}

//Handles any and all PWM sets based on data above from wheel speed sets
void handlePWM() {
    //PWM code, sets the pin modes exactly N% of the time in accordance with the % set in the PWM for that motor variable
    //We skip some ms if we're switching current to make sure we don't have current flow collision with the switches turning off and on

    //Handle right wheels
    if (rBackwardsPrevious != rBackwards) {
        rBackwardsPrevious = rBackwards;
        digitalWrite(pRIGHT_FORWARD, LOW);
        digitalWrite(pRIGHT_BACKWARD, LOW);
    } else {
        if ((time % 100) < rPWM)
            digitalWrite(rBackwards ? pRIGHT_BACKWARD : pRIGHT_FORWARD, HIGH);
        else
            digitalWrite(rBackwards ? pRIGHT_BACKWARD : pRIGHT_FORWARD, LOW);
    }
    //Handle left wheels
    if (lBackwardsPrevious != lBackwards) {
        lBackwardsPrevious = lBackwards;
        digitalWrite(pLEFT_FORWARD, LOW);
        digitalWrite(pLEFT_BACKWARD, LOW);
    } else {
        if ((time % 100) < rPWM)
            digitalWrite(lBackwards ? pLEFT_BACKWARD : pLEFT_FORWARD, HIGH);
        else
            digitalWrite(lBackwards ? pLEFT_BACKWARD : pLEFT_FORWARD, LOW);
    }
}

//Sets the vehicle turning at a specific speed
void setTurnPWM(int speed) {
    setWheelSpeed(true, speed);
    setWheelSpeed(false, -speed);
}

//Sets the vehicle moving front-back at a specific speed
void setStraightPWM(int speed) {
    setWheelSpeed(true, speed);
    setWheelSpeed(false, speed);
}
