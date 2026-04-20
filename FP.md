---
marp: true
theme: default
paginate: true
size: 16:9
style: |
  section.video-slide {
    text-align: center;
  }
  section.video-slide h1 {
    margin-bottom: 0.35em;
  }
---

# CSC 494 Final Presentation  
## Outdoor Cleaning Rover

**Hunter Dreves** 

**CSC 494**  

---

# Project Overview

## Problem Domain
Outdoor pet waste cleanup.

 The goal was to create a rover that can move around outside, perform the task of picking up pet waste, and put it in a bin. This prevents having to do it manually by hand and dealing with the cleanup. This problem combined power distribution, mechanical design, sensing, and control logic in order to come up with a solution.

---

# Solution Domain

A robotic system that combines hardware and software to perform the task of waste cleanup.

My solution uses a small rover with motors, a scoop/dump mechanism powered by a servo, a breadboard housing ESP32 and VL53L0X sensor, a Raspberry Pi, DC convertors for Pi and servo, and a camera to see where the rover is moving.

---

# Progress Since S1P

## At S1P
- chassis was assembled
- Raspberry Pi was set up with camera and SSH
- breadboard and wiring were partially started

## FP
- Week 1: Soldered and wired sensors, started designing parts
- Week 2: Created webpage to interface with rover, started main electrical wiring
- Week 3: Continued creating webpage while waiting for parts
- Week 4: Created wires and wire connectors

---
# Progress Since S1P

## FP
- Week 5: Re-created custom parts, finished all main wiring
- Week 6: Created sketch for ESP32, put everything together
- Week 7: Began testing and working out bugs
- Week 8: Testing and working out bugs
---

## Project links
- **Canvas:** https://nku.instructure.com/courses/88152/pages/individual-progress-hunter-dreves
- **GitHub:** https://github.com/Hdreves/outdoor-cleaning-rover/tree/main

---

<!-- _class: video-slide -->

# Final Demo

<video controls preload="metadata" width="960">
  <source src="./FinalDemo.mp4" type="video/mp4">
</video>

---
# Learning With AI

## AI helped with
- Control logic design
- Debugging motors, sensors, and servo behavior
- Power distribution planning
- Code/setup guidance for integration

## Main lessons
- AI is useful for speeding up debugging
- AI struggles with mechanical designs
- AI is best used as a tool for support

---
# Topic 1

## Control Logic

The main takeaways when it comes to control logic is High level commands (Pi) need to be separated from low level commands (ESP32), pins need to be properly placed on ESP32, and not trying to force too much PWM control. With the Pi and ESP32 separated, debugging becomes much easier because you can isolate your issues. Also, ESP32 is much better at handling real time controls as that's what it was intended to do. Without proper pin placements, control goes out the window. You will run into problems like wrong motor movements, only one side moving, or nothing happening at all. When too many devices are being controlled through PWM, the devices will not function properly. Both devices are competing for timers/frequencies and in most cases one dominates the other. Also, devices use different timers/frequencies. For example, my servo (lower Hz) and DC motors (higher Hz), which lead to only the motors working. 

---
# Topic 2

## Power Distribution

The main takeaways when it comes to power distribution is sharing common ground, using convertors for lower rated electronics, and capacitors to prevent voltage spikes and drops. Without common ground signals are unreliable, sensors won't read or give bad values, random movements occur, or electrical damage. Without the use of convertors, a high voltage battery can fry your circut boards. The lower rated components can't handle the voltage supplied by the battery leading to failures and broken components. Capactitors come in handy for servos because sometimes voltage spikes and suddenly drops. During a voltage spike the capacitor stores that energy, and releases it during a moment of voltage drop.

---

# Issues Encountered and Solutions

## Issue 1: Wiring 
- Sensors not showing
- Wiring space
- Need for specific wires

## Solutions
- Used AI to determine sensor wasn't on shared ground
- Asked AI to help determine the dimensions for the box housing the elctrical components
- Had to cut, strip, and crimp the wires for my use case

--- 

## Issue 2: Hardware/Software Integration
- Motors moving incorrectly
- Values updating improperly
- Wheels continuosly moved after powering
- Servo wouldn't move

## Solutions
- Motors needed to be inverted
- Used AI to diagnose that there was interference when trying to use PWM for both the servo and motors. Moved the motors off of PWM and used digital direction signals through the motor driver instead.
---

## Issue 3: Design
- Initial design was much too complex
- Lack of space
- Reprint parts multiple times

## Soltuions
- Simplified the design
- Made space in any way possible, through creating holes and routing wires through them
- Modified incorrect parts and reprinted

---

# Questions
