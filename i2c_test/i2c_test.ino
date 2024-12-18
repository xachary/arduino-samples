#include <Wire.h>
#include <ssd1306.h>
#include "FaBoGas_CCS811.h"

// OLED 0.96
// 接口：GND->GND、VDD->VCC(5V)、SCK->SCK/A5、SDA->SDA/A4
// iis地址：0x3C
#define SCREEN_WIDTH 128      // 设置OLED宽度,单位:像素
#define SCREEN_LINE_HEIGHT 8  // 设置OLED宽度,单位:像素

FaBoGas faboGas(0x5A);

//int co2;
//int total_voc;
int sensor_val[2];

void setup() {
  Serial.begin(9600);

  Wire.begin();

  // 清屏
  for (int i = 0; i < 100; i++) {
    Serial.print("\n");
  }

  Serial.println("Setup start");

  // 工具初始化

  while (!faboGas.begin()) {
    Serial.println("Sensor Error");
    delay(500);
  }

  Serial.println("Sensor OK");

  delay(500);

  // 初始化OLED
  ssd1306_128x64_i2c_init();
  ssd1306_setFixedFont(ssd1306xled_font6x8);
  ssd1306_clearScreen();

  Serial.println("Setup end");
}

int freeRam() {
  extern int __heap_start, *__brkval;
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

int n = 0;

void loop() {
  Serial.println();

  n++;
  Serial.print("loop:");
  Serial.println(n);

  // Serial.print(F("SRAM left: "));
  // Serial.println(freeRam());

  char numStr[SCREEN_WIDTH] = "";
  itoa(n, numStr, 10);

  ssd1306_printFixedN(0, SCREEN_LINE_HEIGHT * 0, numStr, STYLE_NORMAL, 1);

  if (faboGas.checkStatus()) {
    faboGas.getAlgResultsData(sensor_val);

    Serial.print("TVOC:");
    Serial.print(sensor_val[1]);
    Serial.println("ppb");

    char ppbStr[SCREEN_WIDTH] = "";
    itoa(sensor_val[1], ppbStr, 10);

    ssd1306_printFixedN(0, SCREEN_LINE_HEIGHT * 3, ppbStr, STYLE_NORMAL, 2);
  }

  delay(100);
}