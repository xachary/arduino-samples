#include <Arduino.h>
#include <Wire.h>

// OLED 0.96 库
#include <ssd1306.h>

// OLED 0.96
// 接口：GND->GND、VDD->VCC(5V)、SCK->SCK/A5、SDA->SDA/A4
// 协议：I2C
// 地址：0x3C

namespace SSD_1306 {
const unsigned int SCREEN_WIDTH = 128;
const unsigned int SCREEN_LINE_HEIGHT = 8;
}

namespace Display {

struct _OLED {
  void Init() {
    // 初始化OLED
    Wire.begin();
    ssd1306_128x64_i2c_init();
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_clearScreen();
  }

  void printRaw(unsigned int left, unsigned int top, char* str, unsigned int style = STYLE_NORMAL) {
    ssd1306_printFixed(left, top, str, style);
  }

  void printNRaw(unsigned int left, unsigned int top, char* str, unsigned int style = STYLE_NORMAL) {
    ssd1306_printFixedN(left, top, str, style, 2);
  }

  void print(char* str, unsigned int left, unsigned int top) {
    printRaw(left, SSD_1306::SCREEN_LINE_HEIGHT * top, str);
  }

  void printRight(char* str, int top) {
    uint16_t left = getLeft(str);
    print(str, left, top);
  }

  void drawBuffer(unsigned int left, unsigned int top, uint8_t* buffer) {
    ssd1306_drawBuffer(left, top, 3, 8, buffer);
  }

  void drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
    ssd1306_drawLine(x1, y1, x2, y2);
  }

  void clearBlock(unsigned int left, unsigned int top, unsigned int w, unsigned int h) {
    ssd1306_clearBlock(left, top, w, h);
  }

  void clearBlockCenter(unsigned int left, unsigned int top, unsigned int right) {
    clearBlock(left, top, SSD_1306::SCREEN_WIDTH - left - right, SSD_1306::SCREEN_LINE_HEIGHT);
  }

  unsigned int getTextSize(char* str) {
    return ssd1306_getTextSize(str, 0);
  }

  unsigned int getLeft(char* str) {
    int w = getTextSize(str);
    return SSD_1306::SCREEN_WIDTH - w;
  }

  void clearScreen() {
    ssd1306_clearScreen();
  }
} OLED;

}