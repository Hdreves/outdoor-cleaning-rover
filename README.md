# Outdoor Cleaning Rover

## Project Description

Outdoor Cleaning Rover is a shoebox-sized autonomous robot designed to patrol a patio and sidewalk and collect dog waste using a small “garbage-truck” style scoop and onboard bin. The rover navigates outdoors at low speed, detects obstacles, and executes a pickup sequence when a target is detected. The project uses embedded systems and sensors (ESP32 + Arduino Nano, distance sensors, bumpers, encoders) and is built for reliability and safety in an outdoor environment.

**High-level goals**
- Drive safely on concrete/pavers and avoid obstacles.
- Detect a target area/object and perform a pickup routine.
- Deposit waste into a lined bin for easy removal.

---

## Problem Domain

Pet waste on patios and sidewalks is unpleasant, unsanitary, and easy to miss especially at night or when the yard/patio is large. Owners often delay cleanup, which can create health risks (bacteria/parasites), odors, and messy tracking into the home. A small rover that can periodically patrol and collect waste reduces the burden on owners and keeps outdoor spaces clean.

---

## Features and Requirements

### Features & Requirements

1. **Autonomous Patrol**
   - The rover follows a patrol pattern over a defined area (patio + sidewalk lane).
   - Supports manual “start/stop” via button and/or Wi-Fi command.

2. **Obstacle Avoidance**
   - The rover detects obstacles in front and to the sides and avoids collisions.
   - Includes physical bumper switches as a backup safety layer.

3. **Pickup Mechanism (Garbage-Truck Scoop + Bin)**
   - A front scoop lowers, collects waste, and deposits into a bin.
   - Bin uses a replaceable bag to simplify cleanup.

4. **Target Detection**
   - Onboard detection using sensors and camera to trigger pickup.

5. **Status Feedback**
   - LED indicators for states: Idle, Patrol, Avoiding, Picking Up, Error.
   - Optional Wi-Fi telemetry: battery level, current state, last pickup time.

6. **Power Management**
   - Battery-powered operation with regulated rails for logic and actuators.

We have **5 core features** and **10 requirements**.

**Functional Requirements**
1. The rover shall move forward, reverse, and turn using differential drive.
2. The rover shall stop within 0.5 seconds when a bumper switch is pressed.
3. The rover shall detect obstacles within at least 0.3–1.0 meters in front.
4. The rover shall perform a pickup cycle (lower scoop → collect → dump → raise).
5. The rover shall store collected waste in an onboard bin with a liner.
6. The rover shall indicate its current state with LEDs.
7. The rover shall operate for at least 15 minutes on battery under typical load.
8. The rover shall prevent microcontroller brownouts during servo actuation.
9. The rover shall support a manual emergency stop (switch/button).
10. The rover shall keep motors disabled on boot until explicitly started.

### Non-Functional Requirements

- **Outdoor Robustness:** Wiring must be secured (no loose breadboard-only wiring on a moving rover). Electronics protected from light splashes and dust.
- **Safety:** Low-speed operation; bumpers as physical fail-safe; emergency stop accessible.
- **Reliability:** Power rails must remain stable under motor/servo load; common ground and proper power distribution required.
- **Maintainability:** Modular subsystems (drive, sensing, pickup, power) with clear connectors and labels.
- **Usability:** Simple “one-button” start and clear status indicators.
- **Budget/Availability:** Parts should be readily purchasable, with common maker components.
- **Documentation:** Schematic/wiring diagram and pin map.

---

## Architecture

### System Overview

- **ESP32 (3.3V)** — *Main controller (“brains”)*
  - High-level behavior/state machine
  - Sensor fusion / navigation logic
  - Wi-Fi telemetry (optional)
  - Sends motor/servo commands

- **Arduino Nano (5V)** — *Real-time I/O controller (optional but recommended)*
  - Stable PWM generation for servos
  - Encoder counting / motor timing tasks
  - Receives commands from ESP32 via UART/I2C

- **Motor Driver (Dual H-Bridge)**
  - Drives left and right DC gear motors with PWM + direction

- **Sensors**
  - ToF distance sensors (front/left/right) for obstacle detection
  - Bumper switches (hardware safety)
  - Wheel encoders (for distance/heading estimation)

- **Actuators**
  - DC motors (drive)
  - Servos (scoop lift + optional dump flap)

- **Power**
  - 2S battery → fuse → main switch
  - Buck converter to regulated 5V rail
  - Motors powered from battery rail (through driver), logic from regulated rail
  - Common ground shared across all modules

## Schedule & Milestones

### Sprint 1 — Build Rover Platform and Safe Autonomy

#### Week 1 — Purchases and Structure
- Purchase remaining materials
- 

#### Week 2 — Assembly
- Assemble chassis (motors, wheels, caster, mounting)
- Build power system: battery → fuse → switch → buck converter
- Wire motor driver
- Verify stable 5V rail under load (motors off vs motors on)

#### Week 3 — Sensors
- Mount and wire ToF distance sensors (front/left/right)
- Mount and wire bumper switches (hardware collision fail-safe)
- Implement obstacle avoidance behavior

#### Week 4 — Controls
- Add wheel encoders and implement tick counting
- Straight-line correction and controlled turning
- Tune speed limits for outdoor safety (slow, stable movement)
- Run short outdoor patrol tests and document issues/iterations

---

### Sprint 2 — Pickup System



Link: [Milestone Tracker](link-to-milestones)

