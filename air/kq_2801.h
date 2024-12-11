#include <Arduino.h>

class KQ_2801 {
private:
  int _read_sample_interval = 50;
  int _read_sample_times = 5;
  int _warm_up_seconds = 300;
  int _start_catch = 5;

  float _sum_voltage_air = 0;
  float _sum_resistance_air = 0;
public:
  KQ_2801(int pm, int pb) {
    pin_mg = pm;
    pin_bool = pb;
  }

  int pin_mg = 0;
  int pin_bool = 0;

  int percentage = 0;

  float voltage_air = 0;
  float resistance_air = 0;

  float voltage_current = 0;
  float resistance_current = 0;

  int read_times = 0;
  bool is_warm_up = true;

  void Init() {
    pinMode(pin_bool, INPUT);      //set pin to input
    digitalWrite(pin_bool, HIGH);  //turn on pullup resistors
  }

  // 来源于网络示例/商家文档

  float Read_Voltage() {
    int i;
    float v = 0;

    for (i = 0; i < _read_sample_times; i++) {
      v += analogRead(pin_mg);
      delay(_read_sample_interval);
    }
    v = (v / _read_sample_times) * 5 / 1024;  //单位是V
    return v;
  }

  int Read_Resistance(float v) {
    return (5 * 3900) / v - 3900;
  }

  int Read() {
    read_times++;

    float v = Read_Voltage();
    int r = Read_Resistance(v);

    if (read_times < _warm_up_seconds) {
      if (read_times > _start_catch) {
        _sum_voltage_air += v;
        voltage_air = _sum_voltage_air / (read_times - 5);

        _sum_resistance_air += r;
        resistance_air = _sum_resistance_air / (read_times - 5);
      }
    } else {
      is_warm_up = false;
    }

    voltage_current = v;
    resistance_current = r;

    if (resistance_air > 0) {
      percentage = 100 - resistance_current / resistance_air * 100;
    } else {
      percentage = 0;
    }
    return percentage;
  }
};