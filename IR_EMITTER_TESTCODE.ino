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
const int delayBetweenReads = 2;

void setup() {
  pinMode (LEFT_EMITTER, OUTPUT);
  pinMode (MID_LEFT_EMITTER, OUTPUT);
  pinMode (MID_RIGHT_EMITTER, OUTPUT);
  pinMode (RIGHT_EMITTER, OUTPUT);

  pinMode (LEFT_SEN, INPUT);
  pinMode (MID_LEFT_SEN, INPUT);
  pinMode (MID_RIGHT_SEN, INPUT);
  pinMode (RIGHT_SEN, INPUT);

  Serial.begin (9600);
}

int averageSenValue (int emitterPin, int sensorPin) {
  int total = 0;

  digitalWrite(emitterPin, HIGH);
  for (int i = 0; i < numSamples; i++) {
    total += analogRead(sensorPin);
    delay (delayBetweenReads);
  }
  digitalWrite(emitterPin, LOW);

  return total / numSamples;
}

void loop() {
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

  delay(50);
}
