#include <Arduino.h>

// 来源于网络示例
// https://www.sohu.com/a/694634336_120248280

// 紫外线指数
// 型号：guva_s12sd
// 接口：VCC->VCC(5V)、GND->GND、SIO->A1
// 协议：ADC

#define _Pin_UV A1

namespace Module {

struct _UV {
  float getValue() {
    unsigned int value = analogRead(_Pin_UV);
    return value / 1024 * 3.3;
  }

  String getUnit() {
    return "index";
  }
} UV;

}