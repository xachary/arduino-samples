#include <Arduino.h>
#include <Wire.h>

class CCS_811 {
public:
  CCS_811(int sda, int scl) {
    pinScl = scl;
    pinSda = sda;

    Wire.begin();
  }

  int pinScl = 0;
  int pinSda = 0;

  int value = 0;

  void Read() {
    // Serial.print("CCS_811 Read");

    // // 向从设备＃8请求6个字节
    // Wire.requestFrom(8, 6);
    // // 当从从设备接收到信息时值为true
    // while (Wire.available()) {
    //   // 接受并读取从设备发来的一个字节的数据
    //   char c = Wire.read();
    //   // 向串口打印该字节
    //   Serial.print(c);
    // }
    // // 延时500毫秒
    // delay(500);
  }
};