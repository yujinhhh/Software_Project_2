#include <Servo.h>
#include <math.h>

#define TRIG_PIN 12
#define ECHO_PIN 13
#define SERVO_PIN 10

const int CAR_DETECT_DISTANCE = 30;
const int MAX_ANGLE = 90;   // 서보 최대 열림
const int CLOSED_ANGLE = 0; // 서보 닫힘

const float k = 0.15;       // 시그모이드 완만하게 조정
const float x0 = 15.0;

Servo myservo;

// 시그모이드 함수
float sigmoid(float x) {
  return 1.0 - (1.0 / (1.0 + exp(k * (x - x0))));
}

// 서보 부드럽게 이동 (2~3도씩)
void softServoControl(float currentDistance) {
  int targetAngle;

  if (currentDistance <= CAR_DETECT_DISTANCE) {
    float normalizedValue = sigmoid(currentDistance);
    targetAngle = (int)(normalizedValue * MAX_ANGLE);
  } else {
    targetAngle = CLOSED_ANGLE;
  }

  int currentAngle = myservo.read();
  int step = 3; // 한 번에 움직이는 각도

  if (currentAngle < targetAngle) {
    currentAngle += step;
    if(currentAngle > targetAngle) currentAngle = targetAngle;
  } else if (currentAngle > targetAngle) {
    currentAngle -= step;
    if(currentAngle < targetAngle) currentAngle = targetAngle;
  }

  myservo.write(currentAngle);

  Serial.print("Distance: ");
  Serial.print(currentDistance);
  Serial.print(" cm, Servo Angle: ");
  Serial.println(currentAngle);
}

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  myservo.attach(SERVO_PIN);
  myservo.write(CLOSED_ANGLE);
  delay(500);
}

void loop() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 25000);
  float distance;

  if (duration == 0) {
    distance = CAR_DETECT_DISTANCE + 1;
  } else {
    distance = duration * 0.034 / 2.0;
  }

  softServoControl(distance);

  delay(20); // 루프 속도 유지
}
