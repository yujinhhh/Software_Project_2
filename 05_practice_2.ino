const int LED_PIN_NUM = 7; 

void blinkLED(int count, int onTime_ms, int offTime_ms) {
  for (int i = 0; i < count; i++) {
    // LED 끄기
    digitalWrite(LED_PIN_NUM, HIGH); 
    delay(onTime_ms);               

    // LED 켜기
    digitalWrite(LED_PIN_NUM, LOW);  
    delay(offTime_ms);              
  }
}

void setup() {
  pinMode(LED_PIN_NUM, OUTPUT);
}

void loop() {

  // LED를 1초 동안 켜기
  digitalWrite(LED_PIN_NUM, LOW); // LED 켜기
  delay(1000);                     // 1초 유지

  // 1초 동안 5회 깜빡이기
  // * 5회 깜빡임 = 0.1초 키고 0.1초 끄기 * 5회 = 총 1.0초
  int blink_count = 5;      // 5번 깜박이기
  int on_ms = 100; // 켜져 있는 시간
  int off_ms = 100;// 꺼져 있는 시간
  
  blinkLED(blink_count, on_ms, off_ms);

  // 종료
  digitalWrite(LED_PIN_NUM, HIGH); // LED 끄기

  // 무한 루프
  while (true) { 
  }
}
