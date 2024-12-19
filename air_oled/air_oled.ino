#include <SoftwareSerial.h>
#include <Wire.h>

#include "units.h"

#include "iic_scan.h"

// OLED 0.96 库
#include <ssd1306.h>

// WZ-S 甲醛传感器
#include "WZ.h"

// 温湿度
#include "dht_11.h"

// 二氧化碳
#include "mtp_40_f.h"

// TVOC指数
#include "ccs_811_soft_wire.h"

// 紫外线指数
#include "guva_s12sd.h"

// 时钟
#include <RTClib.h>

// OLED 0.96
// 接口：GND->GND、VDD->VCC(5V)、SCK->SCK/A5、SDA->SDA/A4
// iis地址：0x3C
#define SCREEN_WIDTH 128      // 设置OLED宽度,单位:像素
#define SCREEN_LINE_HEIGHT 8  // 设置OLED宽度,单位:像素

// 工具实例

// 温湿度
// 镂空面：1->VCC(5V)、2->D?、3->留空、4->GND
// DHT_11 dht_11(4);

// 二氧化碳
// 接口：G+->VCC(5V)、G->GND、PWM->D3
MTP_40_F mtp_40_f(3);

// TVOC指数
// 接口：VCC->VCC(5V)、GND->GND、SCL->A5、SDA->A4、WAK->GND
// iis地址：0x5A
CCS811_SoftWire ccs_811(2, 3, 0x5A);

// 紫外线指数
// 接口：VCC->VCC(5V)、GND->GND、SIO->A1
GUVA_S12SD guva_s12sd(A1);

// 接口：VCC->VCC(5V)、GND->GND、CLK->D8、DAT->D9、RST-D10
DS1302 rtc(10, 8, 9);

// 甲醛传感器(WZ-S)
// 接口：5V->VCC(5V)、G->GND、R->RX、T->TX
// SoftwareSerial Serial_WZ(2, 3);
// WZ wz(Serial_WZ);
// WZ::DATA hcho_data;

// PM传感器(fs00905)
// 转接板：红->VCC(5V)、绿->RX、黄->TX(可不接)、黑->GND
// SoftwareSerial Serial_PM(7, 6);
// int pm_buffer_len = 40;
// byte pm_buffer[40] = {};
// bool pm_buffer_done = false;
// int pm1_data = 0;
// int pm2_5_data = 0;
// int pm10_data = 0;

void ccs_811_begin() {
  int ccs_811_timeout = 0;
  while (ccs_811.begin() == false && ccs_811_timeout < 10) {
    ccs_811_timeout++;
  }
  if (ccs_811_timeout >= 10) {
    Serial.println("CCS811 error. Please check wiring.");
  }
}

void setup() {
  Serial.begin(9600);

  Wire.begin();

  // 清屏
  for (int i = 0; i < 100; i++) {
    Serial.print("\n");
  }

  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(13, OUTPUT);

  // 关掉主板的灯L
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  // 工具初始化

  // mtp_40_f.Init();

  // TVOC传感器
  // ccs_811_begin();

  // 时钟初始化
  rtc.begin();
  // 更新时间
  DateTime now = rtc.now();
  DateTime cNow = DateTime(__DATE__, __TIME__);
  if (!(now.year() > cNow.year()
        || now.month() > cNow.month()
        || now.day() > cNow.day()
        || now.hour() > cNow.hour()
        || now.minute() > cNow.minute()
        || now.second() > cNow.second())) {
    rtc.adjust(cNow);
  }

  delay(500);

  // WZ-S甲醛传感器
  // Serial_WZ.begin(9600);
  // wz.passiveMode();  // 被动

  // PM传感器
  // Serial_PM.begin(9600);

  // 初始化OLED
  ssd1306_128x64_i2c_init();
  ssd1306_setFixedFont(ssd1306xled_font6x8);
  ssd1306_clearScreen();

  Serial.println("");
  Serial.println("Loading...");
}

// 执行次数
long n = 0;

int getLeft(char* str) {
  int w = ssd1306_getTextSize(str, 0);
  return SCREEN_WIDTH - w;
}

void print(char* str, int left, int top) {
  ssd1306_printFixed(left, SCREEN_LINE_HEIGHT * top, str, STYLE_NORMAL);
}

void printRight(char* str, int top) {
  uint16_t left = getLeft(str);
  print(str, left, top);
}

void printTitle() {
  // 加载提醒
  char str[SCREEN_WIDTH] = "[Air]";

  int w = ssd1306_getTextSize(str, 0) * 2;

  ssd1306_printFixedN(SCREEN_WIDTH - w, 0, str, STYLE_NORMAL, 1);
}

int printDate(int row, bool isRight) {
  char str[SCREEN_WIDTH] = "";

  char date[SCREEN_WIDTH] = "";

  DateTime now = rtc.now();
  Serial.print("Read:");
  Serial.println(now.tostr(date));

  strncpy(str, date, 10);

  if (isRight) {
    printRight(str, row);
  } else {
    print(str, 0, row);
  }

  Serial.println(str);

  return ssd1306_getTextSize(str, 0);
}

int printTime(int row, bool isRight) {
  char str[SCREEN_WIDTH] = "";

  char time[SCREEN_WIDTH] = "";

  DateTime now = rtc.now();
  Serial.print("Read:");
  Serial.println(now.tostr(time));

  strncpy(str, time + 11, 8);

  if (isRight) {
    printRight(str, row);
  } else {
    print(str, 0, row);
  }

  Serial.println(str);

  return ssd1306_getTextSize(str, 0);
}

int printPower3(int left, int top) {
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
  ssd1306_drawBuffer(left, top, 3, 8, buffer);

  return 4;
}

int printDeg(int left, int top) {
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
  ssd1306_drawBuffer(left, top, 3, 8, buffer);

  return 4;
}

void clearBlock(int left, int right, int top) {
  ssd1306_clearBlock(left, top, SCREEN_WIDTH - left - right, SCREEN_LINE_HEIGHT);
}

int printTVOC(int row, bool isRight) {
  char str[SCREEN_WIDTH] = "";
  strcat(str, "TVOC:");

  char numStr[SCREEN_WIDTH] = "";
  int value = TVOC_Read();
  itoa(value, numStr, 10);
  strcat(str, numStr);

  if (isRight) {
    printRight(str, row);
  } else {
    print(str, 0, row);
  }

  Serial.println(str);

  return ssd1306_getTextSize(str, 0);
}

int printHCHO_UGM3(int value, int row, bool isRight) {
  char str[SCREEN_WIDTH] = "";
  strcat(str, "HCHO:");

  // int num = value;
  // if (num > 1000) {
  //   num = 0;
  // }
  // char numStr[SCREEN_WIDTH] = "";
  // itoa(num, numStr, 10);
  // strcat(str, numStr);
  // strcat(str, "ug/m");

  // if (isRight) {
  //   printRight(str, row);
  // } else {
  //   print(str, 0, row);
  // }

  Serial.println(str);

  int w = ssd1306_getTextSize(str, 0) + 1;

  return ssd1306_getTextSize(str, 0) + printPower3(w, row);
}

int printCO2(int row, bool isRight) {
  char str[SCREEN_WIDTH] = "";
  strcat(str, "CO2:");

  int num = mtp_40_f.value;
  char numStr[SCREEN_WIDTH] = "";
  itoa(num, numStr, 10);
  strcat(str, numStr);
  strcat(str, "ppm");

  if (isRight) {
    printRight(str, row);
  } else {
    print(str, 0, row);
  }

  Serial.println(str);

  return ssd1306_getTextSize(str, 0);
}

int printTemp(int row, bool isRight) {
  char str[SCREEN_WIDTH] = "";
  strcat(str, "Temp:");

  // int num = dht_11.temperature;
  // char numStr[SCREEN_WIDTH] = "";
  // itoa(num, numStr, 10);
  // strcat(str, numStr);

  // strcat(str, " C");

  // int w = ssd1306_getTextSize("C", 0) + 1;

  // if (isRight) {
  //   printRight(str, row);

  //   printDeg(SCREEN_WIDTH - w - 3, row);
  // } else {
  //   print(str, 0, row);

  //   printDeg(w - 3, row);
  // }

  Serial.println(str);

  return ssd1306_getTextSize(str, 0);
}

int printHum(int row, bool isRight) {
  char str[SCREEN_WIDTH] = "";
  strcat(str, "Hum:");

  // int num = dht_11.humidity;
  // char numStr[SCREEN_WIDTH] = "";
  // itoa(num, numStr, 10);
  // strcat(str, numStr);
  // strcat(str, "%");

  // if (isRight) {
  //   printRight(str, row);
  // } else {
  //   print(str, 0, row);
  // }

  Serial.println(str);

  return ssd1306_getTextSize(str, 0);
}

int printUV(int row, bool isRight) {
  char str[SCREEN_WIDTH] = "";
  strcat(str, "UV:");

  int num = guva_s12sd.value;
  char numStr[SCREEN_WIDTH] = "";
  itoa(num, numStr, 10);
  strcat(str, numStr);

  if (isRight) {
    printRight(str, row);
  } else {
    print(str, 0, row);
  }

  Serial.println(str);

  return ssd1306_getTextSize(str, 0);
}

int printPM1(int row, bool isRight) {
  char str[SCREEN_WIDTH] = "";
  strcat(str, "PM1:");

  // int num = pm1_data;
  // char numStr[SCREEN_WIDTH] = "";
  // itoa(num, numStr, 10);
  // strcat(str, numStr);
  // strcat(str, "ug/m");

  // if (isRight) {
  //   printRight(str, row);
  // } else {
  //   print(str, 0, row);
  // }

  Serial.println(str);

  int w = ssd1306_getTextSize(str, 0) + 1;

  return ssd1306_getTextSize(str, 0) + printPower3(w, row);
}
int printPM2_5(int row, bool isRight) {
  char str[SCREEN_WIDTH] = "";
  strcat(str, "PM2.5:");

  // int num = pm2_5_data;
  // char numStr[SCREEN_WIDTH] = "";
  // itoa(num, numStr, 10);
  // strcat(str, numStr);
  // strcat(str, "ug/m");

  // if (isRight) {
  //   printRight(str, row);
  // } else {
  //   print(str, 0, row);
  // }

  Serial.println(str);

  int w = ssd1306_getTextSize(str, 0) + 1;

  return ssd1306_getTextSize(str, 0) + printPower3(w, row);
}
int printPM10(int row, bool isRight) {
  char str[SCREEN_WIDTH] = "";
  strcat(str, "PM10:");

  // int num = pm10_data;
  // char numStr[SCREEN_WIDTH] = "";
  // itoa(num, numStr, 10);
  // strcat(str, numStr);
  // strcat(str, "ug/m");

  // if (isRight) {
  //   printRight(str, row);
  // } else {
  //   print(str, 0, row);
  // }

  Serial.println(str);

  int w = ssd1306_getTextSize(str, 0) + 1;

  return ssd1306_getTextSize(str, 0) + printPower3(w, row);
}

int WZ_Read_Passive_PPB() {
  // if (wz.read(hcho_data)) {
  //   return hcho_data.HCHO_PPB;
  // }
  return 0;
}

int WZ_Read_Active_UGM3() {
  // wz.requestRead();
  // if (wz.readUntil(hcho_data)) {
  //   return hcho_data.HCHO_UGM3;
  // }
  return 0;
}

void PM_Calc() {
  int count = 0;

  bool flag_start = false;
  bool flag_end = false;

  byte byte_val = 0x00;
  byte last_byte_val = 0x00;

  // while (flag_end == false) {
  //   if (Serial_PM.available() > 0) {
  //     char int_val = (char)Serial_PM.read();
  //     byte_val = (byte)int_val;

  //     pm_buffer[count] = byte_val;

  //     if (pm_buffer[count] == 0x4d && last_byte_val == 0x42) {
  //       pm_buffer[0] = 0x42;
  //       count = 1;
  //       flag_start = true;
  //     }
  //     last_byte_val = byte_val;

  //     count++;

  //     if (flag_start) {
  //       if (count >= pm_buffer_len) {
  //         count = 0;
  //         int sum = 0;
  //         for (int i = 0; i < pm_buffer_len - 2; i++) {
  //           sum += pm_buffer[i];
  //         }

  //         int check = pm_buffer[pm_buffer_len - 2] * 256 + pm_buffer[pm_buffer_len - 1];

  //         if (sum == check) {
  //           pm_buffer_done = true;
  //           flag_end = true;
  //         } else {
  //           Serial.println("<<<<<start<<<<<");
  //           for (int i = 0; i < pm_buffer_len; i++) {
  //             Serial.print(pm_buffer[i], HEX);
  //             Serial.print(",");
  //           }
  //           Serial.println("");
  //           Serial.print("sum:");
  //           Serial.println(sum);
  //           Serial.print("check:");
  //           Serial.println(check);
  //           Serial.println(">>>>>error>>>>>");
  //         }
  //       }
  //     }
  //   }
  // }
}

void PM_Read() {
  // if (Serial_PM.isListening()) {
  //   while (pm_buffer_done == false)  //等待数据接收一帧完整
  //   {
  //     PM_Calc();
  //   }
  //   pm_buffer_done = false;

  //   pm1_data = pm_buffer[10] * 256 + pm_buffer[11];
  //   pm2_5_data = pm_buffer[12] * 256 + pm_buffer[13];
  //   pm10_data = pm_buffer[14] * 256 + pm_buffer[15];
  // }
}

int TVOC_Read() {
  int sensor_val[2];
  int value = 0;

  int timeout = 0;
  while (!ccs_811.checkStatus() && timeout < 10) {
    timeout++;
  };
  ccs_811.getAlgResultsData(sensor_val);
  value = sensor_val[1];
  return value;
}

// 执行逻辑
void process() {
  // 读数

  // dht_11.Read();
  guva_s12sd.Read();
  mtp_40_f.Read();

  // Serial_WZ.listen();
  int hcho_UGM3 = WZ_Read_Active_UGM3();

  Serial.println("");
  Serial.print(n);
  Serial.println("次");

  // LCD显示内容
  printTitle();
  clearBlock(printDate(0, false), SCREEN_WIDTH / 2, 0);
  clearBlock(printTime(1, false), SCREEN_WIDTH / 2, 1);
  clearBlock(printTVOC(2, false), printTemp(2, true), 2);
  clearBlock(printHCHO_UGM3(hcho_UGM3, 3, false), printHum(3, true), 3);
  clearBlock(printCO2(4, false), printUV(4, true), 4);
  clearBlock(printPM1(5, false), 0, 5);
  clearBlock(printPM2_5(6, false), 0, 6);
  clearBlock(printPM10(7, false), 0, 7);
}

bool lightOn = true;
bool btnClick = false;

// 阈值
long at = 500000;

// 帧计数
long timer = at;

// 自定义按秒执行
void runWithTimer() {
  // 计数
  timer++;

  // 每秒（大约）执行一次
  if (timer >= at) {
    Serial.println("process");

    // 重新计数
    timer = 0;
    n++;

    // 执行
    process();
  }
}

// 利用PM传感器每秒推送，作为按秒执行
void runWithSerial() {
  // 利用PM传感器每秒推送作为停顿
  // Serial_PM.listen();
  PM_Read();

  n++;

  // 执行
  process();
}

Units::Mode mode = Units::Temp;

int btn1Status = 0;
int btn2Status = 0;

int printChartName(int x, int y, Units::Mode m) {
  char str[SCREEN_WIDTH] = "";
  Units::getModeName(m, str);

  strcat(str, "(");

  ssd1306_printFixed(x, SCREEN_LINE_HEIGHT * y, str, STYLE_NORMAL);

  return ssd1306_getTextSize(str, 0);
}

int printChartUnit(int x, int y, Units::Mode m) {
  char str[SCREEN_WIDTH] = "";
  Units::getUnit(m, str);

  int w = ssd1306_getTextSize(str, 0);

  switch (m) {
    case Units::HCHO:
    case Units::PM1:
    case Units::PM2_5:
    case Units::PM10:
      strcat(str, " )");
      break;
    default: strcat(str, ")");
  }


  ssd1306_printFixed(x, SCREEN_LINE_HEIGHT * y, str, STYLE_NORMAL);

  return w;
}

int printChartRange(int x, int y, int min, int max) {
  char str[SCREEN_WIDTH] = "";
  char minStr[SCREEN_WIDTH] = "";
  char maxStr[SCREEN_WIDTH] = "";
  itoa(min, minStr, 10);
  itoa(max, maxStr, 10);

  strcat(str, minStr);
  strcat(str, "~");
  strcat(str, maxStr);

  ssd1306_printFixed(SCREEN_WIDTH - ssd1306_getTextSize(str, 0), SCREEN_LINE_HEIGHT * y, str, STYLE_NORMAL);

  return ssd1306_getTextSize(str, 0);
}

void printChartTime(int y) {
  char start[SCREEN_WIDTH] = "-23:00";
  char end[SCREEN_WIDTH] = "+22:00";

  ssd1306_printFixed(0, SCREEN_LINE_HEIGHT * y, start, STYLE_NORMAL);
  ssd1306_printFixed(SCREEN_WIDTH - ssd1306_getTextSize(end, 0), SCREEN_LINE_HEIGHT * y, end, STYLE_NORMAL);
}

const int chartHeaderHeight = SCREEN_LINE_HEIGHT * 2;
const int chartHeight = SCREEN_LINE_HEIGHT * 6;  // 最多定义6行
const int chartCount = 24;      // 24小时内，每小时显示一个点

int getX(int value) {
  float p = value / (float)(chartCount - 1);
  int r = floor(p * SCREEN_WIDTH);

  if (r >= 128) {
    r = 127;
  }

  return r;
}

int getY(int min, int max, int value) {
  float width = max - min;
  float v = value - min;
  float p = v / width;
  int r = floor(p * chartHeight - 2);  // 底部留2像素间隙

  if (r <= 0) {
    r = 1;
  }

  return chartHeight - r;
}

// 阶乘
// long power2(int start, int time) {
//   long res = start;
//   for (int i = 1; i < time; i++) {
//     res *= 2;
//   }
//   return res;
// }

void printLine(uint8_t* values) {
  int min = 10000;
  int max = -10000;
  for (int x = 0; x < chartCount; x++) {
    if (values[x] < min) {
      min = values[x];
    } else if (values[x] > max) {
      max = values[x];
    }
  }

  printChartRange(0, 0, min, max);

  // 填满测试
  // for (int y = 2; y < 8; y++) {
  //   for (int x = 0; x < 128; x++) {
  //     uint8_t buffer[1] = { 0xFF };
  //     ssd1306_drawBuffer(x, y, 1, 8, buffer);
  //   }
  //   Serial.println();
  // }

  ssd1306_clearBlock(0, 2, SCREEN_WIDTH, SCREEN_LINE_HEIGHT * 6);

  for (int x = 1; x < chartCount; x++) {
    Serial.println();

    int fx = getX(x - 1);
    int fy = getY(min, max, values[x - 1]);

    int tx = getX(x);
    int ty = getY(min, max, values[x]);

    ssd1306_drawLine(fx, fy + chartHeaderHeight, tx, ty + chartHeaderHeight);
  }
}

void printChart(Units::Mode m) {
  int w1 = printChartName(0, 0, m);
  int w2 = printChartUnit(w1, 0, m);

  switch (m) {
    case Units::HCHO:
    case Units::PM1:
    case Units::PM2_5:
    case Units::PM10:
      printPower3(w1 + w2 + 1, 0);
      break;
    case Units::Temp:
      printDeg(w1 + 3, 0);
      break;
  }

  printChartTime(1);

  uint8_t values[chartCount] = {};

  for (int x = 0; x < chartCount; x++) {
    values[x] = rand() % 10000;
  }

  printLine(values);
}

// 帧
void loop() {
  Serial.print("mode:");
  Serial.println(mode);
  if (!btn1Status && digitalRead(11) == LOW) {
    btn1Status = 1;
  }

  if (btn1Status && digitalRead(11) == HIGH) {
    btn1Status = 2;
  }

  if (!btn2Status && digitalRead(12) == LOW) {
    btn2Status = 1;
  }

  if (btn2Status && digitalRead(12) == HIGH) {
    btn2Status = 2;
  }

  if (btn1Status == 2) {
    btn1Status = 0;

    mode = mode + 1;
    if (mode > Units::ModeLength - 1) {
      mode = Units::Home + 1;
    }

    ssd1306_clearScreen();
  }

  if (btn2Status == 2) {
    btn2Status = 0;

    mode = Units::Home;

    ssd1306_clearScreen();
  }

  if (mode == Units::Home) {
    runWithSerial();
  } else {
    printChart(mode);
  }

  // delay(500);
}