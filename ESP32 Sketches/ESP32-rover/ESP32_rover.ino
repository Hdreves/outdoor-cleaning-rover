#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <ESP32Servo.h>

// =====================================================
// PIN MAP - BREADBOARD WIRING
// =====================================================
// Right motor driver pins (M1 side)
constexpr int PIN_RIGHT_MOTOR_A = D0;   // M1A
constexpr int PIN_RIGHT_MOTOR_B = D1;   // M1B

// Left motor driver pins (M2 side)
constexpr int PIN_LEFT_MOTOR_A  = D2;   // M2A
constexpr int PIN_LEFT_MOTOR_B  = D3;   // M2B

// ToF
constexpr int PIN_I2C_SDA = D4;
constexpr int PIN_I2C_SCL = D5;

// Servo
constexpr int PIN_SERVO = D6;

// RIGHT encoder (M1 side motor, looking from rear of rover)
constexpr int PIN_RIGHT_ENC_A = D7;     // yellow
constexpr int PIN_RIGHT_ENC_B = D8;     // green

// LEFT encoder (M2 side motor, looking from rear of rover)
constexpr int PIN_LEFT_ENC_A  = D9;     // yellow
constexpr int PIN_LEFT_ENC_B  = D10;    // green

// =====================================================
// CONFIG
// =====================================================
constexpr int SERIAL_BAUD = 9600;

// To fix incorrect motor movements
constexpr bool INVERT_LEFT_MOTOR = false;
constexpr bool INVERT_RIGHT_MOTOR = true;

// Servo config
constexpr int SERVO_MIN_US = 500;
constexpr int SERVO_MAX_US = 2500;
constexpr int SERVO_START_DEG = 0;

constexpr int SCOOP_DOWN_DEG = 10;
constexpr int SCOOP_UP_DEG   = 170;
constexpr int SCOOP_DUMP_DEG = 180;

constexpr uint32_t SERVO_UPDATE_MS = 20;
constexpr int SERVO_STEP_DEG = 2;

constexpr uint32_t TOF_UPDATE_MS = 50;

// =====================================================
// GLOBAL STATES
// =====================================================

VL53L0X tof;
bool tofOk = false;

Servo scoopServo;
String inputLine = "";

String driveState = "stopped";
String scoopState = "idle";
String modeState = "manual";
String cameraState = "offline";
String lastCommand = "none";

bool estopState = false;

volatile long leftTicks = 0;
volatile long rightTicks = 0;
int tofDistance = -1;

float currentLeftCmd = 0.0f;
float currentRightCmd = 0.0f;

int servoCurrentDeg = SERVO_START_DEG;
int servoTargetDeg = SERVO_START_DEG;

bool scoopMotionEnabled = false;

uint32_t lastServoUpdateMs = 0;
uint32_t lastTofUpdateMs = 0;

// =====================================================
// ENCODER ISR HELPERS
// =====================================================

void IRAM_ATTR leftEncA_ISR() {
  bool a = digitalRead(PIN_LEFT_ENC_A);
  bool b = digitalRead(PIN_LEFT_ENC_B);
  leftTicks += (a == b) ? 1 : -1;
}

void IRAM_ATTR leftEncB_ISR() {
  bool a = digitalRead(PIN_LEFT_ENC_A);
  bool b = digitalRead(PIN_LEFT_ENC_B);
  leftTicks += (a != b) ? 1 : -1;
}

void IRAM_ATTR rightEncA_ISR() {
  bool a = digitalRead(PIN_RIGHT_ENC_A);
  bool b = digitalRead(PIN_RIGHT_ENC_B);
  rightTicks += (a == b) ? 1 : -1;
}

void IRAM_ATTR rightEncB_ISR() {
  bool a = digitalRead(PIN_RIGHT_ENC_A);
  bool b = digitalRead(PIN_RIGHT_ENC_B);
  rightTicks += (a != b) ? 1 : -1;
}

// =====================================================
// MOTOR CONTROL
// =====================================================

void hardStopMotors() {
  currentLeftCmd = 0.0f;
  currentRightCmd = 0.0f;

  digitalWrite(PIN_LEFT_MOTOR_A, LOW);
  digitalWrite(PIN_LEFT_MOTOR_B, LOW);
  digitalWrite(PIN_RIGHT_MOTOR_A, LOW);
  digitalWrite(PIN_RIGHT_MOTOR_B, LOW);

  driveState = "stopped";
}

void driveMotorPins(int pinA, int pinB, float cmd, bool invert) {
  if (invert) {
    cmd = -cmd;
  }

  cmd = constrain(cmd, -1.0f, 1.0f);

  if (cmd > 0.01f) {
    digitalWrite(pinA, HIGH);
    digitalWrite(pinB, LOW);
  } else if (cmd < -0.01f) {
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, HIGH);
  } else {
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, LOW);
  }
}

void applyDriveOutputs() {
  driveMotorPins(PIN_LEFT_MOTOR_A,  PIN_LEFT_MOTOR_B,  currentLeftCmd,  INVERT_LEFT_MOTOR);
  driveMotorPins(PIN_RIGHT_MOTOR_A, PIN_RIGHT_MOTOR_B, currentRightCmd, INVERT_RIGHT_MOTOR);
}

void setDriveTank(float leftCmd, float rightCmd, const String& newState) {
  if (estopState) {
    return;
  }

  currentLeftCmd = constrain(leftCmd, -1.0f, 1.0f);
  currentRightCmd = constrain(rightCmd, -1.0f, 1.0f);
  applyDriveOutputs();
  driveState = newState;
}

void stopDrive() {
  hardStopMotors();
}

// =====================================================
// SERVO CONTROL
// =====================================================

void setScoopTarget(int targetDeg, const String& stateName) {
  if (estopState) {
    return;
  }

  servoTargetDeg = constrain(targetDeg, 0, 180);
  scoopState = stateName;
  scoopMotionEnabled = true;
}

void stopScoopMotion() {
  scoopMotionEnabled = false;

  if (servoCurrentDeg == SCOOP_DOWN_DEG) {
    scoopState = "down";
  } else if (servoCurrentDeg == SCOOP_UP_DEG) {
    scoopState = "up";
  } else if (servoCurrentDeg == SCOOP_DUMP_DEG) {
    scoopState = "dumped";
  } else {
    scoopState = "idle";
  }
}

void updateServoMotion() {
  uint32_t now = millis();
  if (now - lastServoUpdateMs < SERVO_UPDATE_MS) {
    return;
  }
  lastServoUpdateMs = now;

  if (!scoopMotionEnabled) {
    return;
  }

  hardStopMotors();

  bool moved = false;

  if (servoCurrentDeg < servoTargetDeg) {
    servoCurrentDeg = min(servoCurrentDeg + SERVO_STEP_DEG, servoTargetDeg);
    moved = true;
  } else if (servoCurrentDeg > servoTargetDeg) {
    servoCurrentDeg = max(servoCurrentDeg - SERVO_STEP_DEG, servoTargetDeg);
    moved = true;
  }

  if (moved) {
    scoopServo.write(servoCurrentDeg);
  }

  if (servoCurrentDeg == servoTargetDeg) {
    scoopMotionEnabled = false;

    if (servoTargetDeg == SCOOP_DOWN_DEG) {
      scoopState = "down";
    } else if (servoTargetDeg == SCOOP_UP_DEG) {
      scoopState = "up";
    } else if (servoTargetDeg == SCOOP_DUMP_DEG) {
      scoopState = "dumped";
    }
  }
}

// =====================================================
// TOF
// =====================================================

void initTof() {
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  tof.setTimeout(50);

  if (tof.init()) {
    tofOk = true;
    tof.setMeasurementTimingBudget(20000);
    tof.startContinuous(50);
  } else {
    tofOk = false;
    tofDistance = -1;
  }
}

void updateTof() {
  if (!tofOk) {
    return;
  }

  uint32_t now = millis();
  if (now - lastTofUpdateMs < TOF_UPDATE_MS) {
    return;
  }
  lastTofUpdateMs = now;

  uint16_t mm = tof.readRangeContinuousMillimeters();
  if (!tof.timeoutOccurred()) {
    tofDistance = (int)mm;
  }
}

// =====================================================
// STATUS / JSON
// =====================================================

void sendOk(const String& cmd) {
  Serial.print("{\"ok\":true,\"last_command\":\"");
  Serial.print(cmd);
  Serial.println("\"}");
}

void sendError(const String& msg) {
  Serial.print("{\"ok\":false,\"error\":\"");
  Serial.print(msg);
  Serial.println("\"}");
}

void sendPing() {
  Serial.println("{\"ok\":true,\"device\":\"xiao_esp32c6\",\"baud\":9600}");
}

void sendStatus() {
  long leftCopy, rightCopy;

  noInterrupts();
  leftCopy = leftTicks;
  rightCopy = rightTicks;
  interrupts();

  Serial.print("{");
  Serial.print("\"connected\":true,");
  Serial.print("\"drive\":\""); Serial.print(driveState); Serial.print("\",");
  Serial.print("\"scoop\":\""); Serial.print(scoopState); Serial.print("\",");
  Serial.print("\"mode\":\""); Serial.print(modeState); Serial.print("\",");
  Serial.print("\"camera\":\""); Serial.print(cameraState); Serial.print("\",");
  Serial.print("\"estop\":"); Serial.print(estopState ? "true" : "false"); Serial.print(",");
  Serial.print("\"left_ticks\":"); Serial.print(leftCopy); Serial.print(",");
  Serial.print("\"right_ticks\":"); Serial.print(rightCopy); Serial.print(",");
  Serial.print("\"tof_distance\":"); Serial.print(tofDistance); Serial.print(",");
  Serial.print("\"last_command\":\""); Serial.print(lastCommand); Serial.print("\"");
  Serial.println("}");
}

// =====================================================
// COMMAND PARSING
// =====================================================

float parseSpeedFromCommand(const String& line, float fallbackValue) {
  int lastSpace = line.lastIndexOf(' ');
  if (lastSpace < 0) {
    return fallbackValue;
  }

  String tail = line.substring(lastSpace + 1);
  float value = tail.toFloat();
  if (value <= 0.0f) {
    return fallbackValue;
  }

  return constrain(value, 0.0f, 1.0f);
}

void handleDriveCommand(const String& line) {
  if (estopState) {
    sendError("estop_active");
    return;
  }

  if (line.startsWith("DRIVE FORWARD")) {
    float speed = parseSpeedFromCommand(line, 0.50f);
    setDriveTank(speed, speed, "forward");
    lastCommand = "forward";
    sendOk(lastCommand);
  }
  else if (line.startsWith("DRIVE REVERSE")) {
    float speed = parseSpeedFromCommand(line, 0.50f);
    setDriveTank(-speed, -speed, "reverse");
    lastCommand = "reverse";
    sendOk(lastCommand);
  }
  else if (line.startsWith("TURN LEFT")) {
    float speed = parseSpeedFromCommand(line, 0.40f);
    setDriveTank(-speed, speed, "left");
    lastCommand = "left";
    sendOk(lastCommand);
  }
  else if (line.startsWith("TURN RIGHT")) {
    float speed = parseSpeedFromCommand(line, 0.40f);
    setDriveTank(speed, -speed, "right");
    lastCommand = "right";
    sendOk(lastCommand);
  }
  else if (line == "STOP" || line == "X") {
    stopDrive();
    lastCommand = "stop";
    sendOk(lastCommand);
  }
  else {
    sendError("unknown_drive_command");
  }
}

void handleScoopCommand(const String& line) {
  if (estopState) {
    sendError("estop_active");
    return;
  }

  hardStopMotors();

  if (line == "SCOOP UP") {
    setScoopTarget(SCOOP_UP_DEG, "raising");
    lastCommand = "scoop_up";
    sendOk(lastCommand);
  }
  else if (line == "SCOOP DOWN") {
    setScoopTarget(SCOOP_DOWN_DEG, "lowering");
    lastCommand = "scoop_down";
    sendOk(lastCommand);
  }
  else if (line == "SCOOP DUMP") {
    setScoopTarget(SCOOP_DUMP_DEG, "dumping");
    lastCommand = "dump";
    sendOk(lastCommand);
  }
  else if (line == "SCOOP STOP") {
    stopScoopMotion();
    lastCommand = "scoop_stop";
    sendOk(lastCommand);
  }
  else {
    sendError("unknown_scoop_command");
  }
}

void handleModeCommand(const String& line) {
  if (line == "MODE MANUAL") {
    modeState = "manual";
    lastCommand = "manual";
    sendOk(lastCommand);
  }
  else if (line == "MODE TEST") {
    modeState = "test";
    lastCommand = "test";
    sendOk(lastCommand);
  }
  else if (line == "MODE IDLE") {
    modeState = "idle";
    stopDrive();
    lastCommand = "idle";
    sendOk(lastCommand);
  }
  else {
    sendError("unknown_mode_command");
  }
}

void processCommand(String line) {
  line.trim();
  line.toUpperCase();

  if (line.length() == 0) {
    return;
  }

  // Support raw key aliases too
  if (line == "W") line = "DRIVE FORWARD";
  else if (line == "S") line = "DRIVE REVERSE";
  else if (line == "A") line = "TURN LEFT";
  else if (line == "D") line = "TURN RIGHT";
  else if (line == "X") line = "STOP";

  if (line == "PING") {
    sendPing();
  }
  else if (line == "STATUS") {
    sendStatus();
  }
  else if (line == "ESTOP") {
    estopState = true;
    stopDrive();
    scoopMotionEnabled = false;
    servoTargetDeg = servoCurrentDeg;
    scoopState = "idle";
    lastCommand = "emergency_stop";
    sendOk(lastCommand);
  }
  else if (line == "CLEAR ESTOP") {
    estopState = false;
    lastCommand = "clear_estop";
    sendOk(lastCommand);
  }
  else if (
    line.startsWith("DRIVE FORWARD") ||
    line.startsWith("DRIVE REVERSE") ||
    line.startsWith("TURN LEFT") ||
    line.startsWith("TURN RIGHT") ||
    line == "STOP"
  ) {
    handleDriveCommand(line);
  }
  else if (
    line == "SCOOP UP" ||
    line == "SCOOP DOWN" ||
    line == "SCOOP DUMP" ||
    line == "SCOOP STOP"
  ) {
    handleScoopCommand(line);
  }
  else if (
    line == "MODE MANUAL" ||
    line == "MODE TEST" ||
    line == "MODE IDLE"
  ) {
    handleModeCommand(line);
  }
  else {
    sendError("unknown_command");
  }
}

// =====================================================
// SAFETY
// =====================================================

void updateCommandWatchdog() {
  if (estopState) {
    stopDrive();
    return;
  }

  // Intentionally no auto-stop timeout here.
  // Rover keeps moving until a STOP command is received.
}

// =====================================================
// SETUP / LOOP
// =====================================================

void setup() {
  pinMode(PIN_LEFT_MOTOR_A, OUTPUT);
  pinMode(PIN_LEFT_MOTOR_B, OUTPUT);
  pinMode(PIN_RIGHT_MOTOR_A, OUTPUT);
  pinMode(PIN_RIGHT_MOTOR_B, OUTPUT);

  digitalWrite(PIN_LEFT_MOTOR_A, LOW);
  digitalWrite(PIN_LEFT_MOTOR_B, LOW);
  digitalWrite(PIN_RIGHT_MOTOR_A, LOW);
  digitalWrite(PIN_RIGHT_MOTOR_B, LOW);

  Serial.begin(SERIAL_BAUD);
  delay(100);

  hardStopMotors();
  delay(750);

  scoopServo.setPeriodHertz(50);
  scoopServo.attach(PIN_SERVO, SERVO_MIN_US, SERVO_MAX_US);
  scoopServo.write(SERVO_START_DEG);
  servoCurrentDeg = SERVO_START_DEG;
  servoTargetDeg = SERVO_START_DEG;

  pinMode(PIN_LEFT_ENC_A, INPUT_PULLUP);
  pinMode(PIN_LEFT_ENC_B, INPUT_PULLUP);
  pinMode(PIN_RIGHT_ENC_A, INPUT_PULLUP);
  pinMode(PIN_RIGHT_ENC_B, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(PIN_LEFT_ENC_A), leftEncA_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_LEFT_ENC_B), leftEncB_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_RIGHT_ENC_A), rightEncA_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_RIGHT_ENC_B), rightEncB_ISR, CHANGE);

  initTof();

  hardStopMotors();

  Serial.println("{\"boot\":\"xiao_rover_ready\",\"baud\":9600}");
}

void loop() {
  while (Serial.available() > 0) {
    char c = (char)Serial.read();

    if (c == '\n' || c == '\r') {
      if (inputLine.length() > 0) {
        processCommand(inputLine);
        inputLine = "";
      }
    } else {
      inputLine += c;
    }
  }

  updateServoMotion();
  updateTof();
  updateCommandWatchdog();
}