#include <Servo.h>

// 핀 설정
#define PIN_LED   9
#define PIN_TRIG  12
#define PIN_ECHO  13
#define PIN_SERVO 10

// 초음파 관련 상수
#define SND_VEL_MM_US 0.346
#define INTERVAL 25
#define PULSE_DURATION 10
#define _DIST_MIN 180.0   // 18cm
#define _DIST_MAX 360.0   // 36cm

#define TIMEOUT (INTERVAL * 500)
#define SCALE (0.5 * SND_VEL_MM_US)  // µs → mm 변환

// EMA 필터 가중치
#define _EMA_ALPHA 0.3

float dist_prev = _DIST_MAX;  // 이전 거리
float dist_ema = _DIST_MAX;   // 필터 적용 거리
unsigned long last_sampling_time = 0;

Servo myservo;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  myservo.attach(PIN_SERVO);
  myservo.write(90);  // 초기 중립

  Serial.begin(57600);
  Serial.println("=== 초음파 거리-서보 제어 시작 ===");
}

void loop() {
  // 샘플링 주기 제어
  if (millis() < last_sampling_time + INTERVAL)
    return;

  float dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);
  float dist_filtered;

  // --- 범위 필터 ---
  if ((dist_raw == 0.0) || (dist_raw < _DIST_MIN) || (dist_raw > _DIST_MAX)) {
    dist_filtered = dist_prev;
    digitalWrite(PIN_LED, HIGH); // 범위 밖이면 LED OFF
  } else {
    dist_filtered = dist_raw;
    dist_prev = dist_raw;
    digitalWrite(PIN_LED, LOW); // 범위 안이면 LED ON
  }

  // --- EMA 필터 ---
  dist_ema = (1 - _EMA_ALPHA) * dist_ema + _EMA_ALPHA * dist_filtered;

  // --- 거리→서보각 변환 ---
  int servo_angle;
  if (dist_ema <= _DIST_MIN) {
    servo_angle = 0;
  } else if (dist_ema >= _DIST_MAX) {
    servo_angle = 180;
  } else {
    servo_angle = map(dist_ema, _DIST_MIN, _DIST_MAX, 0, 180);
  }

  // --- 서보 제어 ---
  myservo.write(servo_angle);

  // --- 시리얼 플로터용 출력 ---
  Serial.print("Min:");   Serial.print(_DIST_MIN);
  Serial.print(",dist:");  Serial.print(dist_raw);
  Serial.print(",ema:");   Serial.print(dist_ema);
  Serial.print(",Servo:"); Serial.print(servo_angle);
  Serial.print(",Max:");   Serial.print(_DIST_MAX);
  Serial.println("");

  last_sampling_time += INTERVAL;
}

// 초음파 거리 측정 함수 (단위: mm)
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}
