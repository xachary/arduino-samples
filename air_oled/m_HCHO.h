#include <Arduino.h>
#include <SoftwareSerial.h>

#include "WZ.h"

// 甲醛传感器
// 型号：WZ-S
// 接口：5V->VCC(5V)、G->GND、R->D2(RX)、T->D3(TX)
// 协议：UART

#define _Pin_HCHO_RX 2
#define _Pin_HCHO_TX 3

namespace Module {

SoftwareSerial _serial_hcho(_Pin_HCHO_RX, _Pin_HCHO_TX);

namespace WZ_S {

WZ wz(_serial_hcho);
WZ::DATA data_hcho;

}

struct _HCHO {
  void Init() {
    _serial_hcho.begin(9600);
  }

  int getValue() {
    _serial_hcho.listen();

    WZ_S::wz.requestRead();
    if (WZ_S::wz.readUntil(WZ_S::data_hcho)) {
      return WZ_S::data_hcho.HCHO_UGM3;
    }

    return 0;
  }

  String getUnit() {
    return "ug\/m";
  }
} HCHO;

}