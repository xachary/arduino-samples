#include <Arduino.h>

#include <DHT22.h>

// 温湿度
// 型号：dht_22
// 接口：DAT->D4、VCC->VCC(5V)、GND->GND
// 协议：1-Wire(单总线)

#define _Pin_Temperature_Humidity 4 

namespace Module {

DHT22 _m_dht_22(_Pin_Temperature_Humidity);

struct _Temperature {
  float getValue() {
    return _m_dht_22.getTemperature();
  }

  String getUnit() {
    return " C";
  }
} Temperature;

struct _Humidity {
  float getValue() {
    return _m_dht_22.getHumidity();
  }

  String getUnit() {
    return "%";
  }
} Humidity;

}