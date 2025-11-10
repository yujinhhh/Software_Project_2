#include <Servo.h>

// === Arduino pin assignment ===
#define PIN_IR    A0        // IR sensor at Pin A0
#define PIN_LED   9         // LED pin
#define PIN_SERVO 10        // Servo pin

// === Servo duty range (microseconds) ===
#define _DUTY_MIN  1000     // servo full clockwise position (0 degree)
#define _DUTY_NEU  1500     // servo neutral position (90 degree)
#define _DUTY_MAX  2000     // servo full counter-clockwise position (180 degree)

// === Distance range (mm) ===
#define _DIST_MIN  100.0    // minimum distance 100mm
#define _DIST_MAX  250.0    // maximum distance 250mm

// === EMA filter and loop interval ===
#define EMA_ALPHA  0.1      // smoothing factor (0.1~0.3 적당)
#define LOOP_INTERVAL  30   // loop interval in msec (20ms 이상)

// === Global variables ===
Servo myservo;
unsigned long last_loop_time = 0;   // msec
float dist_ema = _DIST_MIN;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  myservo.attach(PIN_SERVO);
  myservo.writeMicroseconds(_DUTY_NEU);
  Serial.begin(115200);    // 최대 시리얼 속도
}

void loop() {
  unsigned long time_curr = millis();
  if (time_curr < last_loop_time + LOOP_INTERVAL)
    return;
  last_loop_time = time_curr;

  // === Read IR sensor ===
  float a_value = analogRead(PIN_IR);
  float dist_raw = ((6762.0 / (a_value - 9.0)) - 4.0) * 10.0 - 60.0;  // 거리(mm)

  // === Range filter (10cm~25cm) ===
  if (dist_raw < _DIST_MIN) dist_raw = _DIST_MIN;
  if (dist_raw > _DIST_MAX) dist_raw = _DIST_MAX;

  // === LED control ===
  if (dist_raw >= _DIST_MIN && dist_raw <= _DIST_MAX)
    digitalWrite(PIN_LED, HIGH);
  else
    digitalWrite(PIN_LED, LOW);

  // === EMA filter ===
  dist_ema = (EMA_ALPHA * dist_raw) + ((1 - EMA_ALPHA) * dist_ema);

  // === Map distance to servo duty (직접 계산) ===
  int duty = _DUTY_MIN + (int)((dist_ema - _DIST_MIN) * (_DUTY_MAX - _DUTY_MIN) / (_DIST_MAX - _DIST_MIN));

  // === Servo control ===
  myservo.writeMicroseconds(duty);

  // === Serial output ===
  Serial.print("_DUTY_MIN:");  Serial.print(_DUTY_MIN);
  Serial.print(",_DIST_MIN:"); Serial.print(_DIST_MIN);
  Serial.print(",IR:");        Serial.print(a_value);
  Serial.print(",dist_raw:");  Serial.print(dist_raw);
  Serial.print(",ema:");       Serial.print(dist_ema);
  Serial.print(",servo:");     Serial.print(duty);
  Serial.print(",_DIST_MAX:"); Serial.print(_DIST_MAX);
  Serial.print(",_DUTY_MAX:"); Serial.print(_DUTY_MAX);
  Serial.println("");
}
