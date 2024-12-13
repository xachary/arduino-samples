#include <Wire.h>
// OLED 0.96 库
#include <ssd1306.h>

// WZ-S 甲醛传感器
#include "WZ.h"
#include <SoftwareSerial.h>

#include "dht_11.h"   // 温湿度
#include "kq_2801.h"  // TVOC指数

// OLED 0.96
#define SCREEN_WIDTH 128      // 设置OLED宽度,单位:像素
#define SCREEN_LINE_HEIGHT 8  // 设置OLED宽度,单位:像素

// 工具实例

// 镂空面：1->VCC(5V)、2->D?、3->留空、4->GND
DHT_11 dht_11(4);

// 元器件面：1(AO)->A、2(DO)->D?、3(GND)->GND、4(VCC)->VCC(5V)
KQ_2801 kq_2801(A0, 5);

// 甲醛传感器(WZ-S)
// 接口：5V->VCC(5V)、G->GND、R->RX、T->TX
SoftwareSerial Serial_WZ(2, 3);
WZ wz(Serial_WZ);
WZ::DATA hcho_data;

// PM传感器(fs00905)
// 转接板：红->VCC(5V)、绿->RX、黄->TX(可不接)、黑->GND
SoftwareSerial Serial_PM(7, 6);
int pm_buffer_len = 40;
byte pm_buffer[40] = {};
bool pm_buffer_done = false;
int pm1_data = 0;
int pm2_5_data = 0;
int pm10_data = 0;

void setup() {
  Serial.begin(9600);

  // 关掉主板的灯L
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  // 工具初始化

  // TVOC传感器
  kq_2801.Init();

  // WZ-S甲醛传感器
  Serial_WZ.begin(9600);
  // wz.activeMode();  // 主动推送
  wz.passiveMode();  // 被动

  // PM传感器
  Serial_PM.begin(9600);

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

void printDate() {
  char str[SCREEN_WIDTH] = "20??-??-??\0";

  // int num = n % 30;
  // char numStr[2] = "";
  // itoa(num, numStr, 10);
  // if (num < 10) {
  //   strcat(str, "0");
  // }
  // strcat(str, numStr);

  print(str, 0, 0);

  Serial.println(str);
}

void printTime() {
  char str[SCREEN_WIDTH] = "??:??:??\0";

  // int num = n % 60;
  // char numStr[2] = "";
  // itoa(num, numStr, 10);
  // if (num < 10) {
  //   strcat(str, "0");
  // }
  // strcat(str, numStr);

  print(str, 0, 1);

  Serial.println(str);
}

void printPower3(char* str, int left, int top) {
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
}

void printDeg(char* str, int left, int top) {
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
  // ssd1306_fillRect(left, top * SCREEN_LINE_HEIGHT, left + 1, top * SCREEN_LINE_HEIGHT + 1);
}

void printTVOC() {
  char str[SCREEN_WIDTH] = "";
  strcat(str, "TVOC:");

  if (kq_2801.is_warm_up) {
    char subfix[SCREEN_WIDTH] =  "";
    itoa(kq_2801.warm_up_seconds - kq_2801.read_times, subfix, 10);
    strcat(str, subfix);
  } else {
    char percentageStr[4] = "";
    itoa(kq_2801.percentage, percentageStr, 10);
    strcat(str, percentageStr);
    strcat(str, "%");
  }

  //设置光标位置
  print(str, 0, 2);

  Serial.println(str);
}

void printHCHO_PPB(int value) {
  char str[SCREEN_WIDTH] = "";
  strcat(str, "HCHO:");

  int num = value;
  char numStr[SCREEN_WIDTH] =  "";
  itoa(num, numStr, 10);
  strcat(str, numStr);
  strcat(str, "ppb");

  print(str, 0, 4);

  Serial.println(str);
}

void printHCHO_UGM3(int value) {
  char str[SCREEN_WIDTH] = "";
  strcat(str, "HCHO:");

  int num = value;
  if (num > 1000) {
    num = 0;
  }
  char numStr[SCREEN_WIDTH] =  "";
  itoa(num, numStr, 10);
  strcat(str, numStr);
  strcat(str, "ug/m");

  print(str, 0, 3);

  int w = ssd1306_getTextSize(str, 0) + 1;
  printPower3(str, w, 3);

  Serial.println(str);
}

void printCO2() {
  char str[SCREEN_WIDTH] = "";
  strcat(str, "CO2:");

  int num = 9999;
  char numStr[SCREEN_WIDTH] =  "";
  itoa(num, numStr, 10);
  strcat(str, numStr);
  strcat(str, "ppm");

  print(str, 0, 4);

  Serial.println(str);
}

void printTemp() {
  char str[SCREEN_WIDTH] = "";
  strcat(str, "Temp:");

  int num = dht_11.temperature;
  char numStr[SCREEN_WIDTH] =  "";
  itoa(num, numStr, 10);
  strcat(str, numStr);

  strcat(str, " C");

  printRight(str, 2);

  int w = ssd1306_getTextSize("C", 0) + 1;
  printDeg(str, SCREEN_WIDTH - w - 3, 2);

  Serial.println(str);
}

void printHum() {
  char str[SCREEN_WIDTH] = "";
  strcat(str, "Hum:");

  int num = dht_11.humidity;
  char numStr[SCREEN_WIDTH] =  "";
  itoa(num, numStr, 10);
  strcat(str, numStr);
  strcat(str, "%");

  printRight(str, 3);

  Serial.println(str);
}

void printUV() {
  char str[SCREEN_WIDTH] = "";
  strcat(str, "UV:");

  int num = 9999;
  char numStr[SCREEN_WIDTH] =  "";
  itoa(num, numStr, 10);
  strcat(str, numStr);

  printRight(str, 4);

  Serial.println(str);
}

void printPM1() {
  char str[SCREEN_WIDTH] = "";
  strcat(str, "PM1:");

  int num = pm1_data;
  char numStr[SCREEN_WIDTH] =  "";
  itoa(num, numStr, 10);
  strcat(str, numStr);
  strcat(str, "ug/m");

  print(str, 0, 5);

  int w = ssd1306_getTextSize(str, 0) + 1;
  printPower3(str, w, 5);

  Serial.println(str);
}
void printPM2_5() {
  char str[SCREEN_WIDTH] = "";
  strcat(str, "PM2.5:");

  int num = pm2_5_data;
  char numStr[SCREEN_WIDTH] =  "";
  itoa(num, numStr, 10);
  strcat(str, numStr);
  strcat(str, "ug/m");

  print(str, 0, 6);

  int w = ssd1306_getTextSize(str, 0) + 1;
  printPower3(str, w, 6);

  Serial.println(str);
}
void printPM10() {
  char str[SCREEN_WIDTH] = "";
  strcat(str, "PM10:");

  int num = pm10_data;
  char numStr[SCREEN_WIDTH] =  "";
  itoa(num, numStr, 10);
  strcat(str, numStr);
  strcat(str, "ug/m");

  print(str, 0, 7);

  int w = ssd1306_getTextSize(str, 0) + 1;
  printPower3(str, w, 7);

  Serial.println(str);
}

int WZ_Read_Passive_PPB() {
  if (wz.read(hcho_data)) {
    return hcho_data.HCHO_PPB;
  }
  return 0;
}

int WZ_Read_Active_UGM3() {
  wz.requestRead();
  if (wz.readUntil(hcho_data)) {
    return hcho_data.HCHO_UGM3;
  }
  return 0;
}

void PM_Calc() {
  int count = 0;

  bool flag_start = false;
  bool flag_end = false;

  byte byte_val = 0x00;
  byte last_byte_val = 0x00;

  while (flag_end == false) {
    if (Serial_PM.available() > 0) {
      char int_val = (char)Serial_PM.read();
      byte_val = (byte)int_val;

      pm_buffer[count] = byte_val;

      if (pm_buffer[count] == 0x4d && last_byte_val == 0x42) {
        pm_buffer[0] = 0x42;
        count = 1;
        flag_start = true;
      }
      last_byte_val = byte_val;

      count++;

      if (flag_start) {
        if (count >= pm_buffer_len) {
          count = 0;
          int sum = 0;
          for (int i = 0; i < pm_buffer_len - 2; i++) {
            sum += pm_buffer[i];
          }

          int check = pm_buffer[pm_buffer_len - 2] * 256 + pm_buffer[pm_buffer_len - 1];

          if (sum == check) {
            pm_buffer_done = true;
            flag_end = true;
          } else {
            Serial.println("<<<<<start<<<<<");
            for (int i = 0; i < pm_buffer_len; i++) {
              Serial.print(pm_buffer[i], HEX);
              Serial.print(",");
            }
            Serial.println("");
            Serial.print("sum:");
            Serial.println(sum);
            Serial.print("check:");
            Serial.println(check);
            Serial.println(">>>>>error>>>>>");
          }
        }
      }
    }
  }
}

void PM_Read() {
  if (Serial_PM.isListening()) {
    while (pm_buffer_done == false)  //等待数据接收一帧完整
    {
      PM_Calc();
    }
    pm_buffer_done = false;

    pm1_data = pm_buffer[10] * 256 + pm_buffer[11];
    pm2_5_data = pm_buffer[12] * 256 + pm_buffer[13];
    pm10_data = pm_buffer[14] * 256 + pm_buffer[15];
  }
}

// 执行逻辑
void process() {
  // 读数

  dht_11.Read();
  kq_2801.Read();

  Serial_WZ.listen();
  int hcho_UGM3 = WZ_Read_Active_UGM3();
  // int hcho_PPB = WZ_Read_Passive_PPB();

  Serial.println("");
  Serial.print(n);
  Serial.println("秒");

  // LCD显示内容
  printDate();
  printTime();
  printTitle();

  printTVOC();
  printHCHO_UGM3(hcho_UGM3);
  printCO2();
  printPM1();
  printPM2_5();
  printPM10();
  printTemp();
  printHum();
  printUV();

  // 串口日志
  Serial.print(kq_2801.resistance_current);
  Serial.print("Ω/");
  Serial.print(kq_2801.resistance_air);
  if (kq_2801.is_warm_up) {
    Serial.print("Ω");
    Serial.println("（预热中）");
  } else {
    Serial.println("Ω");
  }
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
  Serial_PM.listen();
  PM_Read();

  n++;

  // 执行
  process();
}

// 帧
void loop() {
  runWithSerial();
}