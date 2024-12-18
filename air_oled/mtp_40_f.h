#include <Arduino.h>

// 【PWM 功能详解】
// PWM 的周期是 1004ms
// 起始阶段高电平输出 2ms
// 中部周期 1000ms
// 结束阶段低电平输出 2ms
// 通过 PWM 获得当前 CO2
// 浓度值的计算公式：
// Cppm = 5000*（TH-2ms）/（TH+TL-4ms）
// Cppm 为计算得到的 CO2
// 浓度值，单位是 ppm
// TH 为一个输出周期中输出为高电平的时间
// TL 为一个输出周期中输出为低电平的时间

int MTP_40_F_PIN = 0;

volatile uint32_t MTP_40_F_START = 0;
volatile uint16_t MTP_40_F_DURATION = 0;

// 计算高电平持续时间
void MTP_40_F_INTERRUPT() {
  if (digitalRead(MTP_40_F_PIN) == HIGH) MTP_40_F_START = millis();
  else MTP_40_F_DURATION = millis() - MTP_40_F_START;
}

class MTP_40_F {
public:
  MTP_40_F(int p) {
    MTP_40_F_PIN = p;
  }

  uint16_t value = 0;

  void Init() {
    pinMode(MTP_40_F_PIN, INPUT_PULLUP);

    // 中断服务例程（ISR），当连接到MTP_40_F_PIN的引脚电平变化时，Arduino会调用这个函数。
    attachInterrupt(digitalPinToInterrupt(MTP_40_F_PIN), MTP_40_F_INTERRUPT, CHANGE);
  }

  void Read() {
    value = DurationToPPM();

    // 量程 400ppm ~ 5000ppm
    if (value > 10000) {
      value = 0;
    }
  }

  // 浓度值的计算公式：
  // Cppm = 5000*（TH-2ms）/（TH+TL-4ms）
  // 单位是 ppm
  uint16_t DurationToPPM() {
    return 5000 * (float)(MTP_40_F_DURATION - 2) / (1004 - 4);
  }
};