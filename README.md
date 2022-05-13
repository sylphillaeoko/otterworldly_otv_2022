# 2022 UMD ENES100 Code - Otterworldly OTV
Search terms: University of Maryland, Over-Terrain Vehicle, Arduino

This is the project of the Otterworldly OTV team for the Spring 2022 semester  
Code contributed was almost all by Alex Swisdak, with minor contributions from Mithran Sivakumar, Aidan Ellwanger

## Project Overview
This project is divided into five files:
<pre>

arduino_control       | main file of the program  
navigation_library    | handles basic navigation tasks  
pwm_library           | handles PWM for the OTV  
sensor_library        | handles sensor interface and measurement conversion  
watchdog_library      | handles the movement watchdog  
</pre>

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

## A Note For Future Students
If you have found this code to use as an example, or your professor has shown it to you as such, be aware:
 - This code is not perfect, and navigation _can_ lock up in some very specific conditions even within the simulator
     - Mostly this happens because the interrupts do not properly process the edge of the arena
     - The edge of the arena does not provide a distance notification within the simulator
 - This code assumes that the OTV is handling its own PWM: that there are no motor drivers. This was, retroactively, a terrible design decision on our end
 - This code only provides mission code examples for the water sampling mission as that was our task
