#include <Arduino.h>

// 基于 https://github.com/RobTillaart/MTP40F/blob/master/examples/MTP40F_PWM_demo/MTP40F_PWM_demo.ino
// 由于上面的示例，关于PWM计算，不符合文档下列所属，因此修改之。

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

// PWM端口：3、5、6、9、10、11、13

// 二氧化碳
// 型号：mtp_40_f
// 接口：G+->VCC(5V)、G->GND、PWM->D3
// 协议：PWM

#define _Pin_CO2 3

namespace Module {

namespace MTP_40_F {

unsigned long start = 0;
unsigned long duration = 0;

// 计算高电平持续时间
void Interrupt() {
  if (digitalRead(_Pin_CO2) == HIGH) start = millis();
  else duration = millis() - start;
}

// 浓度值的计算公式：
// Cppm = 5000*（TH-2ms）/（TH+TL-4ms）
// 单位是 ppm
unsigned int DurationToPPM() {
  return 5000 * (float)(duration - 2) / (1004 - 4);
}

}

struct _CO2 {
  void Init() {
    pinMode(_Pin_CO2, INPUT_PULLUP);

    // 中断服务例程（ISR），当连接到引脚电平变化时，Arduino会调用这个函数。
    attachInterrupt(digitalPinToInterrupt(_Pin_CO2), MTP_40_F::Interrupt, CHANGE);
  }

  unsigned int getValue() {
    unsigned int value = MTP_40_F::DurationToPPM();

    // 量程 400ppm ~ 5000ppm
    // 首次计算结果有可能是 65535，因此做一个判断修正。
    if (value > 10000) {
      value = 0;
    }

    return value;
  }

  char unit[6] = "ug\/m";
} CO2;

}
