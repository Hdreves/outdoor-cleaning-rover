# Outdoor Cleaning Rover

## Project Description

Outdoor Cleaning Rover is a shoebox-sized keyboard/webpage controlled robot designed to patrol a patio and sidewalk and collect dog waste using a small “garbage-truck” style scoop. The rover navigates outdoors at low speed and can pickup waste when seen on camera. The project integrates embedded control, sensing, and vision hardware, powered through a LiPo battery.

**High-level goals**
- Drive safely on concrete/pavers.
- Detect a target/object and perform a pickup routine.
- Deposit waste into a bin for easy removal.

---

## Problem Domain

Pet waste on patios and sidewalks is unpleasant, unsanitary, and easy to miss especially at night or when the yard/patio is large. Owners often delay cleanup, which can create health risks (bacteria/parasites), odors, and messy tracking into the home. A small rover that can periodically patrol and collect waste reduces the burden on owners and keeps outdoor spaces clean.

---

## Features and Requirements

### Features & Requirements

1. Remote Control
   - Controlled through a webpage or keyboard

2. Pickup Mechanism (Garbage-Truck Scoop)
   - A front scoop lowers, collects waste, and deposits into a bin

3. Interactive Webpage
   - Updates with rover movements and sensor readings

4. Power Management
   - Battery-powered operation with regulated rails for logic and actuators

---

## Architecture

### System Overview

- **ESP32 (3.3V)** — Main controller (“brains”)
  - High-level behavior/state machine
  - Sensor fusion / navigation logic
  - Receives commands from Raspberry Pi
  - Sends motor/servo commands
 
- **Raspberry Pi** — Web interface, camera streaming, and high-level user interaction
  - Hosts the webpage/control dashboard
  - Streams live camera feed from the Pi Camera
  - Sends user control commands to the ESP32
  - Displays rover status and sensor feedback

- **Motor Driver (Dual H-Bridge)**
  - Drives left and right DC gear motors

- **Sensors**
  - ToF distance sensor for obstacle detection
  - Wheel encoders (for distance/heading estimation)

- **Actuators**
  - DC motors (drive)
  - Servo

- **Power**
  - 3S battery → main switch
  - Buck converter to regulated 5V rail
  - Motors powered from battery rail (through driver), logic from regulated rail
  - Common ground shared across all modules


