#include <Arduino.h>

class HC_SR501 {
public:
  HC_SR501(int p) {
    pin = p;
  }

  int pin = A0;

  int active = false;
  int value = 0;

  void Init() {
    pinMode(pin, INPUT);
  }

  // 来源于网络示例/商家文档
  bool Read() {
    value = analogRead(pin);

    active = value > 150;

    return active;
  }
};