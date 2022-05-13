# Code Explanation File
This file explains in detail the function and thought process behind each part of the code, divided by file.  
This file does **not** cover the simulator code as it is derivate of the main nagivation + watchdog code

## Main File: [arduino_control.cpp](https://github.com/aswisdak/otterworldly_otv_2022/blob/main/arduino_control/arduino_control.ino)
This is the main code file, containing central functions as well as the Arduino `setup()` and `loop()` functions  

### L10-L26: [link](https://github.com/aswisdak/otterworldly_otv_2022/blob/fb40f81672afec5c19dae8134dbc77c88fe8fd98/arduino_control/arduino_control.ino#L10-L26)
Global constant specification for pins: pin specification is done as such to prevent confusion and the use of "magic constants"  
Global constants are optimized out and take no storage space or variable space because of compiler magic

### L28-L33: [link](https://github.com/aswisdak/otterworldly_otv_2022/blob/fb40f81672afec5c19dae8134dbc77c88fe8fd98/arduino_control/arduino_control.ino#L28-L33)
Global constant specification for other things: specification is done as such to prevent confusion and the use of "magic constants"  
Global constants are optimized out and take no storage space or variable space because of compiler magic  
This is for the ARUCO number, and other measurement constants

### L39-L58: [link](https://github.com/aswisdak/otterworldly_otv_2022/blob/fb40f81672afec5c19dae8134dbc77c88fe8fd98/arduino_control/arduino_control.ino#L39-L58)
Any variables that are used globally and are not constant: mission variables used in this file, some navigation intercommunication ones

### L65-L92: [link](https://github.com/aswisdak/otterworldly_otv_2022/blob/fb40f81672afec5c19dae8134dbc77c88fe8fd98/arduino_control/arduino_control.ino#L65-L92)
The arduino `setup()` function, providing multiple utilities:
 - The setup of every pin into the requisite modes
 - The initialization of the mission with the VISION system
 - The first update of the location, and of the location of the mission site

### L94-L121: [link](https://github.com/aswisdak/otterworldly_otv_2022/blob/fb40f81672afec5c19dae8134dbc77c88fe8fd98/arduino_control/arduino_control.ino#L94-L121)
The arduino's `loop()` function, providing multiple utilities:
 - The keeping of the programs's internal clock, used as reference for PWM and any timing
 - The constant connection of the OTV to the vision system
 - The handling of mission vs navigation logic being in control of the OTV
 - The handling of the PWM on every ms as determined by the program's internal clock
 - The handling of the watchdog code via interrupt and the destination updates that result
 - The delay of the program by one (1) ms to keep the internal program clock in check, which gives us a total count of # of active ms

### L126-L151: [link](https://github.com/aswisdak/otterworldly_otv_2022/blob/fb40f81672afec5c19dae8134dbc77c88fe8fd98/arduino_control/arduino_control.ino#L126-L151)
The mission actuation code in its entirety. Almost completely untested, providing:
 - The reading of the sensor value for the depth of the pool and its post to the VISION system comms bus
 - The activiation of the pump for a set time (normally 20s)
 - The color test of the collected water and the salinity test, and its post to the VISION system comms bus
 - The resumption of navigation actions

### L156-L171: [link](https://github.com/aswisdak/otterworldly_otv_2022/blob/fb40f81672afec5c19dae8134dbc77c88fe8fd98/arduino_control/arduino_control.ino#L156-L171)
This is the top-level function for navigation handling, providing:
 - Location updates for any function called from it for this program tick
 - Movement watchdog checks to make sure the OTV is not stuck against a wall
 - General obstacle navigation that allows the OTV to navigate around an obstacle placed anywhere so long as it is possible to do
 - Location arrival updates so other actions can be resumed


## Navigation File: [navigation_library.cpp](https://github.com/aswisdak/otterworldly_otv_2022/blob/main/arduino_control/navigation_library.ino)
This is the navigation code file, handling any and all navigation (not watchdog) tasks

### L16-L85: [link](https://github.com/aswisdak/otterworldly_otv_2022/blob/fb40f81672afec5c19dae8134dbc77c88fe8fd98/arduino_control/navigation_library.ino#L16-L85)
The main obstalce avoidance function. Purpose changes depending on progress through process
 - [L19-L27](https://github.com/aswisdak/otterworldly_otv_2022/blob/fb40f81672afec5c19dae8134dbc77c88fe8fd98/arduino_control/navigation_library.ino#L19-L27): Backs the vehicle up until it is 25cm from the obstacle
 - [L29-L53](https://github.com/aswisdak/otterworldly_otv_2022/blob/fb40f81672afec5c19dae8134dbc77c88fe8fd98/arduino_control/navigation_library.ino#L29-L53): Wobble the OTV back and forth slightly, sensing and recording data on which way the OTV will clear the obstacle faster
 - [L55-L72](https://github.com/aswisdak/otterworldly_otv_2022/blob/fb40f81672afec5c19dae8134dbc77c88fe8fd98/arduino_control/navigation_library.ino#L55-L72): Turn for either a small set distance or until the OTV cannot see an obstacle, whichever happens first
 - [L74-L82](https://github.com/aswisdak/otterworldly_otv_2022/blob/fb40f81672afec5c19dae8134dbc77c88fe8fd98/arduino_control/navigation_library.ino#L74-L82): Drive forward for a set distance (independent of time) and finish obstacle navigation routine

### L88-L111: [link](https://github.com/aswisdak/otterworldly_otv_2022/blob/fb40f81672afec5c19dae8134dbc77c88fe8fd98/arduino_control/navigation_library.ino#L88-L111)
The distance and direction-finder navigation function that determines OTV heading when not avoiding obstacles
 - [L83-L93](https://github.com/aswisdak/otterworldly_otv_2022/blob/fb40f81672afec5c19dae8134dbc77c88fe8fd98/arduino_control/navigation_library.ino#L89-L93): Find the angle between the OTV heading and the object's direction via trigonometry and some math, in radians
 - [L95-L99](https://github.com/aswisdak/otterworldly_otv_2022/blob/fb40f81672afec5c19dae8134dbc77c88fe8fd98/arduino_control/navigation_library.ino#L95-L99): Uses the above information to go either straight or curved, or instead update locations changed from the watchdog
 - [L100-L106](https://github.com/aswisdak/otterworldly_otv_2022/blob/fb40f81672afec5c19dae8134dbc77c88fe8fd98/arduino_control/navigation_library.ino#L100-L106): Changes the position the destination is at back to its original position if it was modified by the watchdog navigation ssytem
