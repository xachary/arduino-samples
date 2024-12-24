#include <Arduino.h>
#include <SoftwareSerial.h>

// 基于
// 并参考 src/docs/FS00905B-集成空气质量传感器v2.1005.pdf

// PM
// 型号：fs00905
// 转接板：红(红线)->VCC(5V)、绿(黑线)->RX(D7)、黄(黄线)->TX(D6,可不接)、黑(绿线)->GND
// 协议：UART

#define _Pin_PM_RX 7
#define _Pin_PM_TX 6
#define _Frame_Start_1_PM 0x42
#define _Frame_Start_2_PM 0x4d
#define _Frame_Len_PM 40

namespace Module {

SoftwareSerial _serial_pm(_Pin_PM_RX, _Pin_PM_TX);

namespace FS_00905 {

// 帧数据
byte buffer[_Frame_Len_PM] = {};

// 帧数据组装完成
bool buffer_done = false;

int data_pm1 = 0;
int data_pm2_5 = 0;
int data_pm10 = 0;

void Calc() {
  unsigned int timeout = 0;

  // 位序号
  int count = 0;

  // 起始
  bool flag_start = false;
  // 结束
  bool flag_end = false;

  // 当前位
  byte byte_val = 0x00;
  // 上一位
  byte last_byte_val = 0x00;

  // 容错10次
  while (flag_end == false && timeout < _Frame_Len_PM * 10) {
    timeout++;

    // 串口可用
    if (_serial_pm.available() > 0) {
      // 按位读数
      char int_val = (char)_serial_pm.read();
      byte_val = (byte)int_val;

      // 帧数据组装
      buffer[count] = byte_val;

      // 起始符1、2
      if (buffer[count] == _Frame_Start_2_PM && last_byte_val == _Frame_Start_1_PM) {
        // 起始符1
        buffer[0] = _Frame_Start_1_PM;
        // 起始符2，更新 位序号
        count = 1;
        // 起始
        flag_start = true;
      }

      // 记录 上一位
      last_byte_val = byte_val;

      // 下一位
      count++;

      // 起始
      if (flag_start) {
        // 读取长度
        if (count >= _Frame_Len_PM) {
          // 重置
          count = 0;

          // 数据校验：
          // 校验码 = 起始符1 + 起始符2 + 数据 01 高八位 + 数据 01 低八位 + ... + 数据 17 低八位

          // 数据校验 左侧 校验码
          int check = buffer[_Frame_Len_PM - 2] * 256 + buffer[_Frame_Len_PM - 1];

          // 数据校验 右侧 汇总
          int sum = 0;
          for (int i = 0; i < _Frame_Len_PM - 2; i++) {
            sum += buffer[i];
          }

          // 校验
          if (sum == check) {
            // 成功/完成
            buffer_done = true;
            flag_end = true;
          } else {
            // 校验失败
            // 打印日志
            // 进行下一轮
            Serial.println("<<<<<start<<<<<");
            for (int i = 0; i < _Frame_Len_PM; i++) {
              Serial.print(buffer[i], HEX);
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

void Read() {
  // 串口可用
  if (_serial_pm.isListening()) {
    unsigned int timeout = 0;
    // 容错10次
    while (buffer_done == false && timeout < 10)  //等待数据接收一帧完整
    {
      timeout++;
      // 计算
      Calc();
    }
    // 重置
    buffer_done = false;

    // 更新数值
    data_pm1 = buffer[10] * 256 + buffer[11];    // UART 数据定义：数据 09 高八位+低八位
    data_pm2_5 = buffer[12] * 256 + buffer[13];  // UART 数据定义：数据 10 高八位+低八位
    data_pm10 = buffer[14] * 256 + buffer[15];   // UART 数据定义：数据 12 高八位+低八位
  }
}

}

struct _PM {
  void Init() {
    _serial_pm.begin(9600);
  }

  void Read() {
    FS_00905::Read();
  }

  String getUnit() {
    return "ug\/m";
  }
} PM;

struct _PM1 {
  int getValue() {
    return FS_00905::data_pm1;
  }
} PM1;

struct _PM2_5 {
  int getValue() {
    return FS_00905::data_pm2_5;
  }
} PM2_5;

struct _PM10 {
  int getValue() {
    return FS_00905::data_pm10;
  }
} PM10;

}