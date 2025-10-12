/*
 * Distance Measurement with Median Filter
 * HC-SR04 Ultrasonic Sensor
 */

// 핀 정의 
#define PIN_TRIG 12         // Trig 핀
#define PIN_ECHO 13         // Echo 핀
#define PIN_LED 9           // LED 핀
#define TRIG PIN_TRIG       
#define ECHO PIN_ECHO       

// 상수 정의 
#define SND_VEL 346.0       // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25         // sampling interval (unit: msec)
#define PULSE_DURATION 10   // minimum pulse duration (unit: usec)

#define _DIST_MIN 30         // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300       // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL)     // coefficient to convert duration to distance

// 중위수 필터 변수 
#define N_MEDIAN 5                    // 윈도우 크기 N (샘플 개수)
float dist_samples[N_MEDIAN];           // N개의 raw 거리를 저장할 배열
int sample_index = 0;                    // 순환 버퍼의 현재 인덱스
int sample_count = 0;                    // 현재까지 저장된 유효 샘플 수

// 상태 변수 
unsigned long last_sampling_time = 0;   // unit: msec
float dist_median;

// 함수 선언
float getDistance_mm(void);
void bubbleSort(float arr[], int n);
float calculateMedian(float arr[], int n);

//  1. 측정 함수 
float getDistance_mm(void) {
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(PULSE_DURATION);
    digitalWrite(TRIG, LOW);

    return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm
}

// 2. 정렬 함수 (버블 정렬) 
void bubbleSort(float arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                // swap arr[j] and arr[j+1]
                float temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

// 3. 중위수 계산 함수
float calculateMedian(float arr[], int n) {
    if (n == 0) return 0.0;

    float temp_arr[N_MEDIAN];
    for (int i = 0; i < n; i++) {
        temp_arr[i] = arr[i];
    }

    bubbleSort(temp_arr, n);

    if (n % 2 != 0) {
        return temp_arr[n / 2];
    } else {
        return (temp_arr[n / 2 - 1] + temp_arr[n / 2]) / 2.0;
    }
}

// 4. SETUP 함수 
void setup() {
    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_TRIG, OUTPUT);
    pinMode(PIN_ECHO, INPUT);
    digitalWrite(PIN_TRIG, LOW);

    Serial.begin(57600);

    for (int i = 0; i < N_MEDIAN; i++) {
        dist_samples[i] = _DIST_MAX;
    }

    last_sampling_time = millis();
}

// 5. LOOP 함수 
void loop() {
    float dist_raw;

    if (millis() < last_sampling_time + INTERVAL)
        return;

    last_sampling_time = millis();

    dist_raw = getDistance_mm();

    dist_samples[sample_index] = dist_raw;
    sample_index = (sample_index + 1) % N_MEDIAN;

    if (sample_count < N_MEDIAN) {
        sample_count++;
    }

    dist_median = calculateMedian(dist_samples, sample_count);

    if ((dist_median < _DIST_MIN) || (dist_median > _DIST_MAX))
        digitalWrite(PIN_LED, HIGH);
    else
        digitalWrite(PIN_LED, LOW);

    Serial.print("raw:");       Serial.print(dist_raw);
    Serial.print(",median:");   Serial.print(dist_median);
    Serial.print(",Min:");      Serial.print(_DIST_MIN);
    Serial.print(",Max:");      Serial.print(_DIST_MAX);
    Serial.println();
}
