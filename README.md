# 2022 UMD ENES100 Code - Otterworldly OTV
Search terms: University of Maryland, Over-Terrain Vehicle, Arduino

This is the project of the Otterworldly OTV team for the Spring 2022 semester
Code contributed was almost all by Alex Swisdak, with minor contributions from Mithran Sivakumar, Aidan Ellwanger

## Project Overview
This project is divided into five files:

arduino_control       | main file of the program
navigation_library    | handles basic navigation tasks
pwm_library           | handles PWM for the OTV
sensor_library        | handles sensor interface and measurement conversion
watchdog_library      | handles the movement watchdog 

## Project Goals
This project is the code for a water sampling OTV defined with the 2022 mission requirements.
If the requirements specified below do not line up, ask the instructor for an archived mission brief if possible.

The OTV must:
    - Travel from its starting area (where it is placed randomly within and at a random angle) to the mission site
    - Collect conductivity (salt vs fresh) and pollution (green versus not) of the water sample
    - Collect 20 mL of water onto the OTV for transport
    - Navigate through two (2) obstacles and under/over the limbo or flat bars to the end of the course

This project is written in arduino C++ other than the simulator code, which is written in C++ adapted for the simulator

## Code Overview
This code does the following:
    - Provides a generalized obstacle-navigation setup that can handle movement to any point on an arbitrary map without issue
    - Provides bare minimum objective fulfillment requirements
    - Provides a memory and program storage space optimized program to fit within the limits of the Arduino Uno
    - Provide all of these with a well-documented, simple, and stylistically consistent format