#include <Arduino.h>

class Buzzer_Active {
public:
  Buzzer_Active(int p) {
    pin = p;
  }

  int pin = A0;

  void Init() {
    pinMode(pin, OUTPUT);
  }

  // 来源于网络示例/商家文档
  void On() {
    digitalWrite(pin, HIGH);
  }

  void Off() {
    digitalWrite(pin, LOW);
  }
};