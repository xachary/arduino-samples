#include <Arduino.h>

class Buzzer_Passive {
public:
  Buzzer_Passive(int p) {
    pin = p;
  }

  int pin = A0;

  void Init() {
    pinMode(pin, OUTPUT);
  }

  // 来源于网络示例/商家文档
  void Sound(int time, int high, int low) {
    for (int i = 0; i < time; i++)  // output a frequency sound
    {
      digitalWrite(pin, HIGH);  // sound
      delay(high);                 //delay1ms
      digitalWrite(pin, LOW);   //not sound
      delay(low);                 //ms delay
    }
  }
};