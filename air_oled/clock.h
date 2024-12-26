#include <Arduino.h>

// 时钟
#include <RTClib.h>

namespace Clock {

// 时钟
// 接口：VCC->VCC(5V)、GND->GND、CLK->D2、DAT->D4、RST-D5
// 协议：SPI
DS1302 _rtc(5, 2, 4);

struct _RTC {
  void Init() {
    // 时钟初始化
    _rtc.begin();
    
    // 更新时间
    DateTime now = _rtc.now();
    DateTime cNow = DateTime(__DATE__, __TIME__);
    if (!(now.year() > cNow.year()
          || now.month() > cNow.month()
          || now.day() > cNow.day()
          || now.hour() > cNow.hour()
          || now.minute() > cNow.minute()
          || now.second() > cNow.second())) {
      _rtc.adjust(cNow);
    }
  }

  DateTime now() {
    return _rtc.now();
  }
} RTC;

}