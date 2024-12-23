#include <Arduino.h>

#include "src/libs/ccs_811_soft_wire.h"

// 来源于网络示例
// https://www.sohu.com/a/694634336_120248280

// TVOC指数
// 型号：ccs_811
// 接口：VCC->VCC(5V)、GND->GND、SDA->D4、SCL->D5、WAK->GND
// 协议：I2C(软)
// 地址：0x5A

#define _Pin_TVOC_SDA 4
#define _Pin_TVOC_SCL 5
#define _Address_TVOC 0x5A

namespace Module {

CCS811_SoftWire _m_ccs_811(_Pin_TVOC_SDA, _Pin_TVOC_SCL, _Address_TVOC);

struct _TVOC {
  void Init() {
    int timeout = 0;
    while (_m_ccs_811.begin() == false && timeout < 10) {
      timeout++;
    }
    if (timeout >= 10) {
      Serial.println("CCS811 error. Please check wiring.");
    }
  }

  int getValue() {
    int vals[2];

    int timeout = 0;
    while (!_m_ccs_811.checkStatus() && timeout < 10) {
      timeout++;
    };
    _m_ccs_811.getAlgResultsData(vals);
    return vals[1];
  }

  String getUnit() {
    return "ppb";
  }
} TVOC;

}