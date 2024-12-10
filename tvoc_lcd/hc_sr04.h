#include <Arduino.h>

class HC_SR04 {
public:
  HC_SR04(int po, int pi) {
    pinOut = po;
    pinIn = pi;
  }

  int pinOut = 0;
  int pinIn = 0;

  int distance = 1000;

  // 来源于网络示例/商家文档
  int Read() {
    digitalWrite(pinOut, LOW);
    delayMicroseconds(2);
    digitalWrite(pinOut, HIGH);
    delayMicroseconds(10);
    digitalWrite(pinOut, LOW);
    float d = pulseIn(pinIn, HIGH) / 58.00;
    if (d > 1000) {
      d = 999;
    }
    distance = d;
    return distance;
  }
};