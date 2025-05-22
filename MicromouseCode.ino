#include <Adafruit_MCP23X17.h>

Adafruit_MCP23X17 mcp;

#define LEFT_EMITTER 2
#define LEFT_SEN A3

#define MID_LEFT_EMITTER 4
#define MID_LEFT_SEN A2

#define MID_RIGHT_EMITTER 5
#define MID_RIGHT_SEN A1

#define RIGHT_EMITTER 6
#define RIGHT_SEN A0

int leftSen;
int midLeftSen;
int midRightSen;
int rightSen;

const int numSamples = 5;
const int delBetweenReads = 2;

int averageSenValue (int emitterPin, int sensorPin) {
  int total = 0;

  mcp.digitalWrite(emitterPin, HIGH);
  for (int i = 0; i < numSamples; i++) {
    total += analogRead(sensorPin);
    delay (delBetweenReads);
  }
  mcp.digitalWrite(emitterPin, LOW);

  return total / numSamples;
}

#define LEFT_MOTOR1 14
#define LEFT_MOTOR2 15
#define RIGHT_MOTOR1 16
#define RIGHT_MOTOR2 10

#define LEFT_ENCO_A 3
#define LEFT_ENCO_B 9
volatile long pulseCountLeft = 0;
long lastPulseCountLeft = 0;
float distanceTraveledLeft = 0.0;

#define RIGHT_ENCO_A 7
#define RIGHT_ENCO_B 8
volatile long pulseCountRight = 0;
long lastPulseCountRight = 0;
float distanceTraveledRight = 0.0;

const float wheel_diameter = 2.2118;
const int PPR = 100;
float distance_per_pulse = (PI * wheel_diameter) / PPR;

int turnDelay = 250;

#define STOP 0
#define RUN 1
int state = STOP;
const int buttonPin = A0;

bool motorState = false;

void leftMotor() {
  int pinLA = digitalRead(LEFT_ENCO_A);
  int pinLB = digitalRead(LEFT_ENCO_B);

  if (pinLA != pinLB) {
    pulseCountLeft++;
  } else {
    pulseCountLeft--;
  }
}

void rightMotor() {
  int pinRA = digitalRead(RIGHT_ENCO_A);
  int pinRB = digitalRead(RIGHT_ENCO_B);

  if (pinRA != pinRB) {
    pulseCountRight++;
  } else {
    pulseCountRight--;
  }
}

void micromouseForward() {
  digitalWrite(LEFT_MOTOR1, HIGH);
  digitalWrite(LEFT_MOTOR2, LOW);
  digitalWrite(RIGHT_MOTOR1, HIGH);
  digitalWrite(RIGHT_MOTOR2, LOW);
}

void micromouseLeft() {
  digitalWrite(LEFT_MOTOR1, LOW);
  digitalWrite(LEFT_MOTOR2, HIGH);
  digitalWrite(RIGHT_MOTOR1, HIGH);
  digitalWrite(RIGHT_MOTOR2, LOW);
}

void micromouseRight() {
  digitalWrite(LEFT_MOTOR1, HIGH);
  digitalWrite(LEFT_MOTOR2, LOW);
  digitalWrite(RIGHT_MOTOR1, LOW);
  digitalWrite(RIGHT_MOTOR2, HIGH);
}

void micromouseStop() {
  digitalWrite(LEFT_MOTOR1, LOW);
  digitalWrite(LEFT_MOTOR2, LOW);
  digitalWrite(RIGHT_MOTOR1, LOW);
  digitalWrite(RIGHT_MOTOR2, LOW);
}

void setup() {
  mcp.pinMode (LEFT_EMITTER, OUTPUT);
  mcp.pinMode (MID_LEFT_EMITTER, OUTPUT);
  mcp.pinMode (MID_RIGHT_EMITTER, OUTPUT);
  mcp.pinMode (RIGHT_EMITTER, OUTPUT);

  pinMode (LEFT_SEN, INPUT);
  pinMode (MID_LEFT_SEN, INPUT);
  pinMode (MID_RIGHT_SEN, INPUT);
  pinMode (RIGHT_SEN, INPUT);

  pinMode(LEFT_MOTOR1, OUTPUT);
  pinMode(LEFT_MOTOR2, OUTPUT);
  pinMode(RIGHT_MOTOR1, OUTPUT);
  pinMode(RIGHT_MOTOR2, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(LEFT_ENCO_A), leftMotor, CHANGE);
  attachInterrupt(digitalPinToInterrupt(LEFT_ENCO_B), leftMotor, CHANGE);

  attachInterrupt(digitalPinToInterrupt(RIGHT_ENCO_A), rightMotor, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RIGHT_ENCO_B), rightMotor, CHANGE);

  distanceTraveledLeft = 0.0;
  distanceTraveledRight = 0.0;

  pinMode(buttonPin, INPUT);

  Serial.begin(9600);
}

void loop() {
  if (millis() - lastPulseCountLeft >= 1000) {
    lastPulseCountLeft = millis();
  }

  if (millis() - lastPulseCountRight >= 1000) {
    lastPulseCountRight = millis();
  }

  distanceTraveledLeft = pulseCountLeft * distance_per_pulse;
  distanceTraveledRight = pulseCountRight * distance_per_pulse;

  leftSen = averageSenValue (LEFT_EMITTER, LEFT_SEN);
  midLeftSen = averageSenValue (MID_LEFT_EMITTER, MID_LEFT_SEN);
  midRightSen = averageSenValue (MID_RIGHT_EMITTER, MID_RIGHT_SEN);
  rightSen = averageSenValue (RIGHT_EMITTER, RIGHT_SEN);

  // Print sensor values
  Serial.print("LeftVal: ");
  Serial.print(leftSen);
  Serial.print(" MidLeftVal: ");
  Serial.print(midLeftSen);
  Serial.print(" MidRightVal: ");
  Serial.print(midRightSen);
  Serial.print(" RightVal: ");
  Serial.println(rightSen);

  if (digitalRead(buttonPin) == HIGH) {
    if (state == STOP) {
      state = RUN;
      motorState = true;
      micromouseForward();
    } else {
      state = STOP;
      motorState = false;
      micromouseStop();
    }
    delay (300);
  }

  switch (state) {
    case RUN:
      if (motorState) {
        if ((midLeftSen > 100) || (midRightSen > 100)) {
          if (leftSen > 100) {
            Serial.println ("Moving Right.");
            micromouseRight();
          } else if (rightSen > 100) {
            Serial.println ("Moving Left.");
            micromouseLeft();
          }
        } else {
          Serial.println ("Moving Forward.");
          micromouseForward();
        }
      }

      break;

    case STOP:
      micromouseStop();
      delay (200);

      state = STOP;
      break;
  }
}
