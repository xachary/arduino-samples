#include <MemoryFree.h>

// SD 卡
#include <SPI.h>
#include <SD.h>

// 时钟
#include <RTClib.h>

// >>>>>>>>>> 模块 >>>>>>>>>>

#include "m_HCHO.h"
#include "m_Temperature_Tumidity.h"
#include "m_UV.h"
#include "m_CO2.h"
#include "m_TVOC.h"
#include "m_PM.h"

#include "modes.h"
#include "buttons.h"
#include "display.h"
#include "clock.h"
#include "storage.h"

// <<<<<<<<<< 模块 <<<<<<<<<<

void setup() {
  Serial.begin(9600);

  // 清屏
  for (unsigned int i = 0; i < 100; i++) {
    Serial.print("\n");
  }

  Clock::RTC.Init();

  Module::CO2.Init();
  Module::PM.Init();

  // 以下设备务必先连接

  // Display::OLED.Init();

  // char sd_error[SSD_1306::SCREEN_WIDTH] = "";
  // if (!Storage::SD_Card.Init(10, sd_error)) {
  //   Serial.println(sd_error);
  //   Display::OLED.print(0, SSD_1306::SCREEN_LINE_HEIGHT * 0, sd_error);

  //   不允许启动
  //   while (true)
  //     ;
  // }
}

// 执行次数
long n = 0;

Modes::Mode mode = Modes::Home;

unsigned int printPower3(unsigned int left, unsigned int top) {
  // 绘制立方"³"符号
  // 位图方向：从左往右、从下往上
  // 0 0 0
  // 0 0 0
  // 0 0 0
  // 1 1 1
  // 0 0 1
  // 1 1 1
  // 0 0 1
  // 1 1 1
  // 第一列 00010101 -> 0x15
  // 第二列 00010101 -> 0x15
  // 第三列 00011111 -> 0x1F
  // js转换示例：parseInt('00011111',2) -> (31).toString(16) -> 1f
  uint8_t buffer[3] = { 0x15, 0x15, 0x1F };
  Display::OLED.drawBuffer(left, top, buffer);

  return 4;
}

unsigned int printDeg(unsigned int left, unsigned int top) {
  // 绘制"°"符号
  // 位图方向：从左往右、从下往上
  // 0 0 0
  // 0 0 0
  // 0 0 0
  // 0 0 0
  // 0 0 0
  // 0 1 0
  // 1 0 1
  // 0 1 0
  uint8_t buffer[3] = { 0x02, 0x05, 0x02 };
  Display::OLED.drawBuffer(left, top, buffer);

  return 4;
}

void printTitle() {
  char str[SSD_1306::SCREEN_WIDTH] = "[Air]";

  unsigned int w = Display::OLED.getTextSize(str) * 2;

  Display::OLED.printNRaw(SSD_1306::SCREEN_WIDTH - w, 0, str);
}

unsigned int printMem() {
  char str[SSD_1306::SCREEN_WIDTH] = "";

  strcat(str, "Mem");

  unsigned int w = Display::OLED.getTextSize(str);

  Display::OLED.printRaw(SSD_1306::SCREEN_WIDTH - w, SSD_1306::SCREEN_LINE_HEIGHT * 0, str);

  return w;
}

unsigned int printMemPercent() {
  char str[SSD_1306::SCREEN_WIDTH] = "";

  char numStr[SSD_1306::SCREEN_WIDTH] = "";
  dtostrf(100.0 - freeMemory() / (float)2048 * (float)100, 0, 0, numStr);

  strcat(str, numStr);
  strcat(str, "%");

  unsigned int w = Display::OLED.getTextSize(str);

  Display::OLED.printRaw(SSD_1306::SCREEN_WIDTH - w, SSD_1306::SCREEN_LINE_HEIGHT * 1, str);

  return w;
}

unsigned int printDate(unsigned int row, bool isRight) {
  char str[SSD_1306::SCREEN_WIDTH] = "";

  char date[SSD_1306::SCREEN_WIDTH] = "";

  DateTime now = Clock::RTC.now();
  now.tostr(date);

  strncpy(str, date, 10);

  if (isRight) {
    Display::OLED.printRight(str, row);
    ;
  } else {
    Display::OLED.print(str, 0, row);
  }

  Serial.println(str);

  return Display::OLED.getTextSize(str);
}

unsigned int printTime(unsigned int row, bool isRight) {
  char str[SSD_1306::SCREEN_WIDTH] = "";

  char time[SSD_1306::SCREEN_WIDTH] = "";

  DateTime now = Clock::RTC.now();
  now.tostr(time);

  strncpy(str, time + 11, 8);

  if (isRight) {
    Display::OLED.printRight(str, row);
    ;
  } else {
    Display::OLED.print(str, 0, row);
  }

  Serial.println(str);

  return Display::OLED.getTextSize(str);
}


unsigned int printTVOC(float value, unsigned int row, bool isRight) {
  char str[SSD_1306::SCREEN_WIDTH] = "";
  strcat(str, "TVOC:");

  char numStr[SSD_1306::SCREEN_WIDTH] = "";
  itoa(value, numStr, 10);
  strcat(str, numStr);

  if (isRight) {
    Display::OLED.printRight(str, row);
    ;
  } else {
    Display::OLED.print(str, 0, row);
  }

  Serial.println(str);

  return Display::OLED.getTextSize(str);
}

unsigned int printHCHO_UGM3(unsigned int value, unsigned int row, bool isRight) {
  char str[SSD_1306::SCREEN_WIDTH] = "";
  strcat(str, "HCHO:");

  unsigned int num = value;
  if (num > 1000) {
    num = 0;
  }
  char numStr[SSD_1306::SCREEN_WIDTH] = "";
  itoa(num, numStr, 10);
  strcat(str, numStr);
  strcat(str, "ug/m");

  if (isRight) {
    Display::OLED.printRight(str, row);;
  } else {
    Display::OLED.print(str, 0, row);
  }

  Serial.println(str);

  unsigned int w = Display::OLED.getTextSize(str) + 1;

  return Display::OLED.getTextSize(str) + printPower3(w, row);
}

unsigned int printCO2(unsigned int value, unsigned int row, bool isRight) {
  char str[SSD_1306::SCREEN_WIDTH] = "";
  strcat(str, "CO2:");

  char numStr[SSD_1306::SCREEN_WIDTH] = "";
  itoa(value, numStr, 10);
  strcat(str, numStr);
  strcat(str, "ppm");

  if (isRight) {
    Display::OLED.printRight(str, row);
    ;
  } else {
    Display::OLED.print(str, 0, row);
  }

  Serial.println(str);

  return Display::OLED.getTextSize(str);
}

unsigned int printTemp(float value, unsigned int row, bool isRight) {
  char str[SSD_1306::SCREEN_WIDTH] = "";
  strcat(str, "Temp:");

  char numStr[SSD_1306::SCREEN_WIDTH] = "";
  itoa(value, numStr, 10);
  strcat(str, numStr);

  strcat(str, " C");

  unsigned int w = Display::OLED.getTextSize("C") + 1;

  if (isRight) {
    Display::OLED.printRight(str, row);
    ;

    printDeg(SSD_1306::SCREEN_WIDTH - w - 3, row);
  } else {
    Display::OLED.print(str, 0, row);

    printDeg(w - 3, row);
  }

  Serial.println(str);

  return Display::OLED.getTextSize(str);
}

unsigned int printHum(float value, unsigned int row, bool isRight) {
  char str[SSD_1306::SCREEN_WIDTH] = "";
  strcat(str, "Hum:");

  char numStr[SSD_1306::SCREEN_WIDTH] = "";
  itoa(value, numStr, 10);
  strcat(str, numStr);
  strcat(str, "%");

  if (isRight) {
    Display::OLED.printRight(str, row);
    ;
  } else {
    Display::OLED.print(str, 0, row);
  }

  Serial.println(str);

  return Display::OLED.getTextSize(str);
}

unsigned int printUV(unsigned int value, unsigned int row, bool isRight) {
  char str[SSD_1306::SCREEN_WIDTH] = "";
  strcat(str, "UV:");

  char numStr[SSD_1306::SCREEN_WIDTH] = "";
  itoa(value, numStr, 10);
  strcat(str, numStr);

  if (isRight) {
    Display::OLED.printRight(str, row);
    ;
  } else {
    Display::OLED.print(str, 0, row);
  }

  Serial.println(str);

  return Display::OLED.getTextSize(str);
}

unsigned int printPM1(unsigned int value, unsigned int row, bool isRight) {
  char str[SSD_1306::SCREEN_WIDTH] = "";
  strcat(str, "PM1:");

  char numStr[SSD_1306::SCREEN_WIDTH] = "";
  itoa(value, numStr, 10);
  strcat(str, numStr);
  strcat(str, "ug/m");

  if (isRight) {
    Display::OLED.printRight(str, row);
    ;
  } else {
    Display::OLED.print(str, 0, row);
  }

  Serial.println(str);

  unsigned int w = Display::OLED.getTextSize(str) + 1;

  return Display::OLED.getTextSize(str) + printPower3(w, row);
}
unsigned int printPM2_5(unsigned int value, unsigned int row, bool isRight) {
  char str[SSD_1306::SCREEN_WIDTH] = "";
  strcat(str, "PM2.5:");

  char numStr[SSD_1306::SCREEN_WIDTH] = "";
  itoa(value, numStr, 10);
  strcat(str, numStr);
  strcat(str, "ug/m");

  if (isRight) {
    Display::OLED.printRight(str, row);
    ;
  } else {
    Display::OLED.print(str, 0, row);
  }

  Serial.println(str);

  unsigned int w = Display::OLED.getTextSize(str) + 1;

  return Display::OLED.getTextSize(str) + printPower3(w, row);
}
unsigned int printPM10(unsigned int value, unsigned int row, bool isRight) {
  char str[SSD_1306::SCREEN_WIDTH] = "";
  strcat(str, "PM10:");

  char numStr[SSD_1306::SCREEN_WIDTH] = "";
  itoa(value, numStr, 10);
  strcat(str, numStr);
  strcat(str, "ug/m");

  if (isRight) {
    Display::OLED.printRight(str, row);
    ;
  } else {
    Display::OLED.print(str, 0, row);
  }

  Serial.println(str);

  unsigned int w = Display::OLED.getTextSize(str) + 1;

  return Display::OLED.getTextSize(str) + printPower3(w, row);
}

// >>>>>>>>>> 折线图 >>>>>>>>>>

namespace ChartLine {

unsigned int printChartName(unsigned int x, unsigned int y, Modes::Mode m) {
  char str[SSD_1306::SCREEN_WIDTH] = "";
  Modes::getModeName(m, str);

  strcat(str, "(");

  Display::OLED.print(x, SSD_1306::SCREEN_LINE_HEIGHT * y, str);

  return Display::OLED.getTextSize(str);
}

unsigned int printChartUnit(unsigned int x, unsigned int y, Modes::Mode m) {
  char str[SSD_1306::SCREEN_WIDTH] = "";
  Modes::getUnit(m, str);

  unsigned int w = Display::OLED.getTextSize(str);

  switch (m) {
    case Modes::HCHO:
    case Modes::PM1:
    case Modes::PM2_5:
    case Modes::PM10:
      strcat(str, " )");
      break;
    default: strcat(str, ")");
  }


  Display::OLED.print(x, SSD_1306::SCREEN_LINE_HEIGHT * y, str);

  return w;
}

unsigned int printChartRange(unsigned int x, unsigned int y, int min, int max) {
  char str[SSD_1306::SCREEN_WIDTH] = "";
  char minStr[SSD_1306::SCREEN_WIDTH] = "";
  char maxStr[SSD_1306::SCREEN_WIDTH] = "";
  itoa(min, minStr, 10);
  itoa(max, maxStr, 10);

  strcat(str, minStr);
  strcat(str, "~");
  strcat(str, maxStr);

  Display::OLED.print(SSD_1306::SCREEN_WIDTH - Display::OLED.getTextSize(str), SSD_1306::SCREEN_LINE_HEIGHT * y, str);

  return Display::OLED.getTextSize(str);
}

void printChartTime(unsigned int y) {
  char start[SSD_1306::SCREEN_WIDTH] = "-23:00";
  char end[SSD_1306::SCREEN_WIDTH] = "+22:00";

  Display::OLED.print(0, SSD_1306::SCREEN_LINE_HEIGHT * y, start);
  Display::OLED.print(SSD_1306::SCREEN_WIDTH - Display::OLED.getTextSize(end), SSD_1306::SCREEN_LINE_HEIGHT * y, end);
}

const unsigned int chartHeaderHeight = SSD_1306::SCREEN_LINE_HEIGHT * 2;
const unsigned int chartHeight = SSD_1306::SCREEN_LINE_HEIGHT * 6;  // 最多定义6行
const unsigned int chartCount = 24;                                 // 24小时内，每小时显示一个点

unsigned int getX(int value) {
  float p = value / (float)(chartCount - 1);
  unsigned int r = floor(p * SSD_1306::SCREEN_WIDTH);

  if (r >= 128) {
    r = 127;
  }

  return r;
}

unsigned int getY(int min, int max, int value) {
  float width = max - min;
  float v = value - min;
  float p = v / width;
  unsigned int r = floor(p * chartHeight - 2);  // 底部留2像素间隙

  if (r <= 0) {
    r = 1;
  }

  return chartHeight - r;
}

// 阶乘
// long power2(unsigned int start, unsigned int time) {
//   long res = start;
//   for (unsigned int i = 1; i < time; i++) {
//     res *= 2;
//   }
//   return res;
// }

void printLine(uint8_t* values) {
  int min = 10000;
  int max = -10000;
  for (unsigned int x = 0; x < chartCount; x++) {
    if (values[x] < min) {
      min = values[x];
    } else if (values[x] > max) {
      max = values[x];
    }
  }

  printChartRange(0, 0, min, max);

  // 填满测试
  // for (unsigned int y = 2; y < 8; y++) {
  //   for (unsigned int x = 0; x < 128; x++) {
  //     uint8_t buffer[1] = { 0xFF };
  //     Display::OLED.drawBuffer(x, y, 1, 8, buffer);
  //   }
  //   Serial.println();
  // }

  Display::OLED.clearBlock(0, 2, SSD_1306::SCREEN_WIDTH, SSD_1306::SCREEN_LINE_HEIGHT * 6);

  for (unsigned int x = 1; x < chartCount; x++) {
    Serial.println();

    unsigned int fx = getX(x - 1);
    unsigned int fy = getY(min, max, values[x - 1]);

    unsigned int tx = getX(x);
    unsigned int ty = getY(min, max, values[x]);

    Display::OLED.drawLine(fx, fy + chartHeaderHeight, tx, ty + chartHeaderHeight);
  }
}

void printChart(Modes::Mode m) {
  unsigned int w1 = printChartName(0, 0, m);
  unsigned int w2 = printChartUnit(w1, 0, m);

  switch (m) {
    case Modes::HCHO:
    case Modes::PM1:
    case Modes::PM2_5:
    case Modes::PM10:
      printPower3(w1 + w2 + 1, 0);
      break;
    case Modes::Temp:
      printDeg(w1 + 3, 0);
      break;
  }

  printChartTime(1);

  uint8_t values[chartCount] = {};

  for (unsigned int x = 0; x < chartCount; x++) {
    values[x] = rand() % 10000;
  }

  printLine(values);
}

}

// <<<<<<<<<< 折线图 <<<<<<<<<<

// 执行逻辑
void process() {
  n++;

  Serial.println("");
  Serial.print(n);
  Serial.println("次");

  Serial.print("mode:");
  char modeName[5] = "";
  Modes::getModeName(mode, modeName);
  Serial.println(modeName);

  Serial.print("HCHO:");
  Serial.print(Module::HCHO.getValue());
  Serial.println(Module::HCHO.getUnit());
  Serial.print("Temperature:");
  Serial.print(Module::Temperature.getValue());
  Serial.println(Module::Temperature.getUnit());
  Serial.print("Humidity:");
  Serial.print(Module::Humidity.getValue());
  Serial.println(Module::Humidity.getUnit());
  Serial.print("UV:");
  Serial.print(Module::UV.getValue());
  Serial.println(Module::UV.getUnit());
  Serial.print("CO2:");
  Serial.print(Module::CO2.getValue());
  Serial.println(Module::CO2.getUnit());
  Serial.print("TVOC:");
  Serial.print(Module::TVOC.getValue());
  Serial.println(Module::TVOC.getUnit());

  Module::PM.Read();
  Serial.print("PM1:");
  Serial.print(Module::PM1.getValue());
  Serial.println(Module::PM.getUnit());
  Serial.print("PM2_5:");
  Serial.print(Module::PM2_5.getValue());
  Serial.println(Module::PM.getUnit());
  Serial.print("PM10:");
  Serial.print(Module::PM10.getValue());
  Serial.println(Module::PM.getUnit());

  // 以下设备务必先连接

  // 显示内容
  // Display::OLED.clearBlockCenter(printDate(0, false), printMem(), 0);
  // Display::OLED.clearBlockCenter(printTime(1, false), printMemPercent(), 1);
  // Display::OLED.clearBlockCenter(printTVOC(Module::TVOC.getValue(), 2, false), printTemp(Module::Temperature.getValue(), 2, true), 2);
  // Display::OLED.clearBlockCenter(printHCHO_UGM3(Module::HCHO.getValue(), 3, false), printHum(Module::Humidity.getValue(), 3, true), 3);
  // Display::OLED.clearBlockCenter(printCO2(Module::CO2.getValue(), 4, false), printUV(Module::UV.getValue(), 4, true), 4);
  // Display::OLED.clearBlockCenter(printPM1(Module::PM1.getValue(), 5, false), 0, 5);
  // Display::OLED.clearBlockCenter(printPM2_5(Module::PM1.getValue(), 6, false), 0, 6);
  // Display::OLED.clearBlockCenter(printPM10(Module::PM1.getValue(), 7, false), 0, 7);

  // 记录日志
  // DateTime now = Clock::RTC.now();
  // Storage::SD_Card.WriteLog(now);
  // Storage::SD_Card.ReadLog(now);
}

void loop() {
  if (Buttons::Btn_1.getValue()) {
    mode = mode + 1;
    if (mode > Modes::Length - 1) {
      mode = Modes::Home + 1;
    }

    Display::OLED.clearScreen();
  }

  if (Buttons::Btn_2.getValue()) {
    mode = Modes::Home;

    Display::OLED.clearScreen();
  }

  if (mode == Modes::Home) {
    process();
  } else {
    ChartLine::printChart(mode);
  }

  delay(1000);
}