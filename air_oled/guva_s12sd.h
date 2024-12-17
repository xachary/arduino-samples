#include <Arduino.h>

// 来源于网络示例
// https://www.sohu.com/a/694634336_120248280
class GUVA_S12SD {
public:
  GUVA_S12SD(int p) {
    pin = p;
  }

  int pin = 0;
  int value = 0;
  float voltage = 0;

  void Read() {
    value = analogRead(pin);
    voltage = value / 1024 * 3.3;
  }
};