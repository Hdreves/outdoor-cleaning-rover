---
marp: true
title: Outdoor Cleaning Rover — Project Plan
theme: default
paginate: true
size: 16:9
---

# Outdoor Cleaning Rover
## Project Plan Presentation (PPP)

**Shoebox-sized autonomous rover that patrols patio/sidewalk and collects dog waste** 

**Hunter Dreves
 CSC 494**

---

## Problem & Motivation

- Pet waste on patios/sidewalks is unsanitary and easy to miss
- Manual cleanup is time-consuming and often delayed
- A small rover can:
  - Patrol routinely
  - Avoid obstacles safely
  - Perform a pickup cycle

---
## Topics to Learn With AI

**Software:** Finite State Machines
  - Keeps the rovers movements organized and predictable.
  Prevents actions from overlapping and leadiing to errors.
  

**Hardware:** Power Distribution
  - Motors and servos create voltage dips.
  Without solid power distribution there 
  can be an increase in glitches.

---

## Sprint 1

- **How to power everything safely**
  - battery basics, turning the robot on/off, and keeping the electronics stable

- **How to make the rover move reliably**
  - drive forward/backward/turn and keep it controllable at a slow, safe speed

- **How to connect and read sensors**
  - distance sensors to “see” obstacles and bumpers as an emergency stop

- **How to test and troubleshoot**

---

## Sprint 2

- **Build the rover and pickup system**
  - a shoebox sized robot with a front scoop and a small bin so the rover can collect waste and store it

- **Make the scoop move automatically**
  - program the scoop to go down, collect, dump into the bin, then reset

- **Connect everything into one complete system**
  - the rover drives, stops when needed, picks up, then continues patrolling


---
## Sprint 2 (cont.)
- **Test outside and improve reliability**
  - run repeated outdoor tests and fix movement issues

- **Full Demo**
  - complete working system that collects and disposes waste

---
## Project Goals

**1. Safe Autonomous Movement**
- Drives on concrete and wood at low speed
- Avoids obstacles and stops on contact

**2. Reliable Pickup Mechanism**
- Scoop lowers, collects, dumps into bin, resets

**3. Demo**
- Successful pick up and removal

---

## Key Features

1. **Autonomous Patrol** 

2. **Obstacle Avoidance** 

3. **Physical Fail-Safe** 

4. **Pickup Sequence** 

5. **Status Feedback** 

---

## System Architecture

- **ESP32 (3.3V):** main logic + state machine + Wi-Fi
- **Arduino Nano (5V):** optional real-time I/O (servo PWM, encoders)
- **Motor Driver:** dual H-bridge for DC motors
- **Sensors:** ToF (front/left/right), bumpers, encoders
- **Actuators:** DC motors, servos
- **Power:** 2S battery → fuse → switch → buck converter (5V)

