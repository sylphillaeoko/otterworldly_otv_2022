/**
 * SENSOR INTERFACE LIBRARY FILE
 */


//Pings and waits for a response from the navigation ultrasonic sensor, return cm
short pingNavigation() {
    //Ping and measure navigation ultrasonic sensor distance
    digitalWrite(pTRIGGER_NAV, LOW);
    digitalWrite(pTRIGGER_NAV, HIGH);
    delayMicroseconds(20);
    digitalWrite(pTRIGGER_NAV, LOW);
    //Return analog input value for the sensor
    return (short)(pulseIn(pINPUT_NAV, HIGH) * 0.017182131);
}

//Pings and waits for a response from the depth ultrasonic sensor, return mm
short pingDepth() {
    //Ping and measure water depth ultrasonic sensor distance
    digitalWrite(pTRIGGER_DEPTH, LOW);
    digitalWrite(pTRIGGER_DEPTH, HIGH);
    delayMicroseconds(20);
    digitalWrite(pINPUT_DEPTH, LOW);
    //Return analog input value for the sensor after math
    return SENSOR_HEIGHT - BASE_HEIGHT - (short)(pulseIn(pINPUT_DEPTH, HIGH) * 0.17182131);
}

//Gets the blue value from the color sensor
short pingBlue() {
    digitalWrite(pCOLOR_S1,LOW);
    digitalWrite(pCOLOR_S2,HIGH);
    delayMicroseconds(20);
    digitalWrite(pCOLOR_S1,HIGH);
    digitalWrite(pCOLOR_S2,LOW);
    //Return the read analog input
    return analogRead(pINPUT_COLOR);  
}

//Gets the red value from the color sensor
short pingRed() {
    digitalWrite(pCOLOR_S1,LOW);
    digitalWrite(pCOLOR_S2,LOW);
    delayMicroseconds(20);      
    digitalWrite(pCOLOR_S1,HIGH);
    digitalWrite(pCOLOR_S2,HIGH);
    //Return the read analog input
    return analogRead(pINPUT_COLOR);
}

//Gets the green value from the color sensor
short pingGreen() {
    digitalWrite(pCOLOR_S1,HIGH);
    digitalWrite(pCOLOR_S2,HIGH);
    delayMicroseconds(20);
    digitalWrite(pCOLOR_S1,LOW);
    digitalWrite(pCOLOR_S2,LOW);
    //Return the read analog input
    return analogRead(pINPUT_COLOR);
}
