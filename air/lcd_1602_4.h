#include <Arduino.h>

class LCD_1602_4 {
public:
  LCD_1602_4(int RS, int RW, int EN, int DB[4]) {
    LCD1602_RS = RS;
    LCD1602_RW = RW;
    LCD1602_EN = EN;
    for (int i = 0; i < 4; i++) {
      LCD1602_DB[i] = DB[i];
    }
  }
  int LCD1602_RS = 0;                  // 寄存器选择（Register Select）引脚。
  int LCD1602_RW = 0;                  // 读/写（Read/Write）选择引脚。
  int LCD1602_EN = 0;                  // 使能（Enable）引脚。
  int LCD1602_DB[4] = { 0, 0, 0, 0 };  // 数据引脚。

  // 来源于网络示例/商家文档
  
  void Init() {
    pinMode(LCD1602_RS, OUTPUT);
    pinMode(LCD1602_RW, OUTPUT);
    pinMode(LCD1602_EN, OUTPUT);

    for (int i = 0; i < sizeof(LCD1602_DB); i++) {
      pinMode(LCD1602_DB[i], OUTPUT);
    }

    Command_Write(0x28);  // 使用4线接口，2行显示，字符大小为5x7点阵。
    delay(50);
    Command_Write(0x06);  // 显示不移动，每次写入数据时地址自动加1。
    delay(50);
    Command_Write(0x0c);  // 打开显示，关闭光标，不闪烁。
    delay(50);
    Command_Write(0x80);  // 设置数据写入地址为起始位置，通常为第0行第0列。
    delay(50);

    Clear();
  }

  void Clear() {
    Command_Write(0x01);  // 清除LCD显示器的显示内容，并将数据指针返回到起始地址。
    delay(50);
  }

  void Clear_Line(int line) {
    Write_String(0, line, "                ");
  }

  void Command_Write(int command) {
    int i, temp;
    digitalWrite(LCD1602_RS, LOW);
    digitalWrite(LCD1602_RW, LOW);
    digitalWrite(LCD1602_EN, LOW);

    temp = command & 0xf0;
    for (i = LCD1602_DB[0]; i <= 9; i++) {
      digitalWrite(i, temp & 0x80);
      temp <<= 1;
    }

    digitalWrite(LCD1602_EN, HIGH);
    delayMicroseconds(1);
    digitalWrite(LCD1602_EN, LOW);

    temp = (command & 0x0f) << 4;
    for (i = LCD1602_DB[0]; i <= 10; i++) {
      digitalWrite(i, temp & 0x80);
      temp <<= 1;
    }

    digitalWrite(LCD1602_EN, HIGH);
    delayMicroseconds(1);
    digitalWrite(LCD1602_EN, LOW);
  }

  void Data_Write(int dat) {
    int i = 0, temp;
    digitalWrite(LCD1602_RS, HIGH);
    digitalWrite(LCD1602_RW, LOW);
    digitalWrite(LCD1602_EN, LOW);

    temp = dat & 0xf0;
    for (i = LCD1602_DB[0]; i <= 9; i++) {
      digitalWrite(i, temp & 0x80);
      temp <<= 1;
    }

    digitalWrite(LCD1602_EN, HIGH);
    delayMicroseconds(1);
    digitalWrite(LCD1602_EN, LOW);

    temp = (dat & 0x0f) << 4;
    for (i = LCD1602_DB[0]; i <= 10; i++) {
      digitalWrite(i, temp & 0x80);
      temp <<= 1;
    }

    digitalWrite(LCD1602_EN, HIGH);
    delayMicroseconds(1);
    digitalWrite(LCD1602_EN, LOW);
  }

  void SET_XY(int x, int y) {
    int address;
    if (y == 0) address = 0x80 + x;
    else address = 0xC0 + x;
    Command_Write(address);
  }

  void Write_String(int X, int Y, char *s) {
    SET_XY(X, Y);
    //设置地址
    while (*s)  //写字符串
    {
      Data_Write(*s);

      s++;
    }
  }

  void Write_Char(int x, int y, int dat) {
    SET_XY(x, y);
    Data_Write(dat);
  }
};