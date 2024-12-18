#include "SparkFunCCS811.h"

#include <Wire.h>

// OLED 0.96 库
#include <ssd1306.h>

#define CCS811_ADDR 0x5A

// OLED 0.96
// 接口：GND->GND、VDD->VCC(5V)、SCK->SCK/A5、SDA->SDA/A4
// iis地址：0x3C
#define SCREEN_WIDTH 128      // 设置OLED宽度,单位:像素
#define SCREEN_LINE_HEIGHT 8  // 设置OLED宽度,单位:像素

CCS811 ccs_811(CCS811_ADDR);

void printTitle() {
  // 加载提醒
  char str[SCREEN_WIDTH] = "[Air]";

  int w = ssd1306_getTextSize(str, 0) * 2;

  ssd1306_printFixedN(SCREEN_WIDTH - w, 0, str, STYLE_NORMAL, 1);
}

void setup() {
  Serial.begin(9600);

  Wire.begin();

  for (int i = 0; i < 50; i++) {
    Serial.print("\n");
  }

  // 初始化OLED
  ssd1306_128x64_i2c_init();
  ssd1306_setFixedFont(ssd1306xled_font6x8);
  ssd1306_clearScreen();

  if (ccs_811.begin() == false) {
    Serial.print("CCS811 error. Please check wiring. Freezing...");
    while (1)
      ;
  }
}

void loop() {
  Serial.println("loop");

  printTitle();

  if (ccs_811.dataAvailable()) {
    //If so, have the sensor read and calculate the results.
    //Get them later
    ccs_811.readAlgorithmResults();

    int num = ccs_811.getTVOC();
    char numStr[SCREEN_WIDTH] = "";
    itoa(num, numStr, 10);

    Serial.println(numStr);

    ssd1306_clearScreen();
    ssd1306_printFixedN(0, 0, numStr, STYLE_NORMAL, 1);
  }

  delay(500);
}
