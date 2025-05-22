#define RIGHT_SEN A0
#define MID_RIGHT_SEN A1
#define MID_LEFT_SEN A2
#define LEFT_SEN A3

#define LEFT_MOTOR1 5
#define LEFT_MOTOR2 2
#define RIGHT_MOTOR1 6
#define RIGHT_MOTOR2 4

#define LEFT_ENCO_A 7
#define LEFT_ENCO_B 8

#define RIGHT_ENCO_A 3
#define RIGHT_ENCO_B 9

#define STOP 0
#define RUN 1

#define ERR 20

#define DISTANCE_TRIG 750

#define SIDE_TRIG_L 900
#define SIDE_TRIG_R 900

#define CEN_TRIG_L 945
#define CEN_TRIG_R 910

const float wheel_diameter = 2.2118;
const int PPR = 100;
float distance_per_pulse = (PI * wheel_diameter) / PPR;

volatile long pulseCountLeft = 0;
long lastPulseCountLeft = 0;
float distanceTraveledLeft = 0.0;

volatile long pulseCountRight = 0;
long lastPulseCountRight = 0;
float distanceTraveledRight = 0.0;

float targetDistance = 0.0;
float distanceToObject = 0.0;

float tolerance = 1.0;

int sensorReading[10];
int totalLeftReading = 0;

int rightSensorReading[10];
int totalRightReading = 0;

const int buttonPin = 10;

int state = STOP;

int leftSen = 0;
int midLeftSen = 0;
int midRightSen = 0;
int rightSen = 0;

int combinedAveragedSenValue;

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

void controlMotorLeft() {
  if (combinedAveragedSenValue >= DISTANCE_TRIG) {
    if (distanceTraveledLeft < targetDistance - tolerance) {
      digitalWrite(LEFT_MOTOR1, LOW);
      digitalWrite(LEFT_MOTOR2, HIGH);
    }
  }
}

void controlMotorRight() {
  if (combinedAveragedSenValue >= DISTANCE_TRIG) {
    if (distanceTraveledRight < targetDistance - tolerance) {
      digitalWrite(RIGHT_MOTOR1, LOW);
      digitalWrite(RIGHT_MOTOR2, HIGH);
    }
  }
}

//void controlMotor(int senValue, int distanceTrigg, float& distanceTraveled, float targetDistance,
//                  float tolerance, int motorPin1, int motorPin2, String motorName) {
//  if (senValue >= distanceTrigg) {
//    if (distanceTraveled < targetDistance - tolerance) {
//      digitalWrite(motorPin1, LOW);
//      digitalWrite(motorPin2, HIGH);
//      Serial.print(motorName);
//      Serial.println(" motor moving forward.");
//    }
//  }
//}

void setup() {
  pinMode(LEFT_MOTOR1, OUTPUT);
  pinMode(LEFT_MOTOR2, OUTPUT);
  pinMode(RIGHT_MOTOR1, OUTPUT);
  pinMode(RIGHT_MOTOR2, OUTPUT);

  pinMode(LEFT_SEN, INPUT);

  pinMode(buttonPin, INPUT);

  attachInterrupt(digitalPinToInterrupt(LEFT_ENCO_A), leftMotor, CHANGE);
  attachInterrupt(digitalPinToInterrupt(LEFT_ENCO_B), leftMotor, CHANGE);

  attachInterrupt(digitalPinToInterrupt(RIGHT_ENCO_A), rightMotor, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RIGHT_ENCO_B), rightMotor, CHANGE);

  distanceTraveledLeft = 0.0;
  distanceTraveledRight = 0.0;

  Serial.begin(9600);
}

void loop() {
  totalLeftReading = 0;

  for (int i = 0; i < 10; i++) {
    sensorReading[i] = analogRead(LEFT_SEN);
    totalLeftReading += sensorReading[i];
    delay(10);
  }

  for (int i = 0; i < 10; i++) {
    rightSensorReading[i] = analogRead(RIGHT_SEN);
    totalRightReading += rightSensorReading[i];
    delay(10);
  }

  int averagedSenValueLeft = totalLeftReading / 10;
  int averagedSenValueRight = totalRightReading / 10;

  combinedAveragedSenValue = (averagedSenValueLeft + averagedSenValueRight) / 2;
  targetDistance = map(combinedAveragedSenValue, 0, 1023, 0, 100);
  Serial.print("Target Distance (mapped): ");
  Serial.println(targetDistance);

  distanceTraveledLeft = pulseCountLeft * distance_per_pulse;
  distanceTraveledRight = pulseCountRight * distance_per_pulse;

  if (millis() - lastPulseCountLeft >= 1000) {
    lastPulseCountLeft = millis();
    Serial.print("LeftDistance Traveled: ");
    Serial.print(distanceTraveledLeft);
    Serial.println(" cm");
  }

  if (millis() - lastPulseCountRight >= 1000) {
    lastPulseCountRight = millis();
    Serial.print("RightDistance Traveled: ");
    Serial.print(distanceTraveledRight);
    Serial.println(" cm");
  }

  //  unsigned long currentMillis = millis();

  if (digitalRead(buttonPin)) {
    state++;
    if (state == 2) state = 0;
    delay(300);
  }

  switch (state) {
    case RUN:
      controlMotorRight();
      controlMotorLeft();

      //      controlMotor(combinedAveragedSenValue, DISTANCE_TRIG, distanceTraveledLeft, targetDistance, tolerance, LEFT_MOTOR1, LEFT_MOTOR2, "Left");
      //      controlMotor(combinedAveragedSenValue, DISTANCE_TRIG, distanceTraveledRight, targetDistance, tolerance, RIGHT_MOTOR1, RIGHT_MOTOR2, "Right");
      state = RUN;
      break;

    case STOP:
      digitalWrite(LEFT_MOTOR1, LOW);
      digitalWrite(LEFT_MOTOR2, LOW);
      digitalWrite(RIGHT_MOTOR1, LOW);
      digitalWrite(RIGHT_MOTOR2, LOW);
      state = STOP;
      break;
  }
}
