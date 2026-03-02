---
marp: true
size: 4:3
paginate: true
---

# S1P  
## Outdoor Cleaning Rover

Hunter Dreves  

---

## Sprint 1 Progress

 **Chassis assembly**
- Rover chassis assembled (mechanical foundation complete)

 **Raspberry Pi Cam set up**
- SSH access configured for headless
- Camera connected and tested

 **Electronics groundwork**
- Breadboard layout for ESP (waiting on sensors should be in on 3/4)
- Planned separation of logic, servo, and motor power rails for safety

---

## Sprint 1 Progress

**Learned about hardware components and reliable power**
- 3s Lipo Battery (expected 3/4)
- 35 Kg servo (scoop and lift)
- 25 Kg servo (dump)
- VL53L0X TOF Sensors (2 meters, expected 3/4)
- 6V Buck converter for servos
- 5V Buck convertor for Pi
- Fuses
- Raspberry Pi (with Camera Module 3)

---

## Sprint 1 Progress
- Raspberry Pi (detection and navigation logic)
- ESP32-C6 (servos)



---

## Sprint 1 Learning with AI

### 1) Driving Controls
- Learned how control can be split between:
  - High-level decisions on Raspberry Pi (state machine)
  - Low-level motor/servo control (ESP32)

### 2) Power and Safe Wiring
- Learned importance of:
  - Separate power rails (motor vs servo vs logic)
  - Common ground
  - Fusing and switching
  - Avoiding brownouts

---

## What Changed From My Original Plan?

---


## Sprint 2 Final Goal

By end of Sprint 2, I will deliver a rover that:

- Manual control 
- Streams camera video from Raspberry Pi  
- Reads distance sensors for obstacle avoidance  
- Actuates the scoop mechanism on command  

**Stretch goals**
- Semi-autonomous navigation
- Target Detection

---

## Sprint 2 Plan

- Build 3-rail power architecture:
  - Motor rail (battery → fuse → switch → motor driver → motors)
  - Servo rail (battery → fuse → switch → 6V buck → servos)
  - Logic rail (battery → fuse → switch → 5V buck/USB-C → Pi + logic)
- Common ground + verify stability under load

---

## Sprint 2 Plan
- Wire VL53L0X ToF sensors on I2C
- Build UI for control
- Controlled tests


---


## Questions?


