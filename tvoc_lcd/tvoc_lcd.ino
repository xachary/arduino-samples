#include <DFRobot_DHT11.h>

long n = 0;

/************************Hardware Related Macros************************************/
#define MG_PIN (A0)  //define which analog input channel you are going to use
#define BOOL_PIN (13)
int RS_AIR_R_MAX;  //定义空气中传感器RS的阻值单位Ω
float RS_GAS_R;    //被测气体中的传感器阻值RS_GAS_R单位Ω
float RS_AIR_V_MIN;
/***********************Software Related Macros************************************/
#define READ_SAMPLE_INTERVAL (50)  //define how many samples you are going to take in normal operation
#define READ_SAMPLE_TIMES (5)      //define the time interval(in milisecond) between each samples in \
                                   //normal operation
#define READ_WARMUP (300)
/*****************************Globals***********************************************/

int LCD1602_RS = 12;
int LCD1602_RW = 11;
int LCD1602_EN = 10;
int DB[] = { 6, 7, 8, 9 };

void LCD_Command_Write(int command) {
  int i, temp;
  digitalWrite(LCD1602_RS, LOW);
  digitalWrite(LCD1602_RW, LOW);
  digitalWrite(LCD1602_EN, LOW);

  temp = command & 0xf0;
  for (i = DB[0]; i <= 9; i++) {
    digitalWrite(i, temp & 0x80);
    temp <<= 1;
  }

  digitalWrite(LCD1602_EN, HIGH);
  delayMicroseconds(1);
  digitalWrite(LCD1602_EN, LOW);

  temp = (command & 0x0f) << 4;
  for (i = DB[0]; i <= 10; i++) {
    digitalWrite(i, temp & 0x80);
    temp <<= 1;
  }

  digitalWrite(LCD1602_EN, HIGH);
  delayMicroseconds(1);
  digitalWrite(LCD1602_EN, LOW);
}

void LCD_Data_Write(int dat) {
  int i = 0, temp;
  digitalWrite(LCD1602_RS, HIGH);
  digitalWrite(LCD1602_RW, LOW);
  digitalWrite(LCD1602_EN, LOW);

  temp = dat & 0xf0;
  for (i = DB[0]; i <= 9; i++) {
    digitalWrite(i, temp & 0x80);
    temp <<= 1;
  }

  digitalWrite(LCD1602_EN, HIGH);
  delayMicroseconds(1);
  digitalWrite(LCD1602_EN, LOW);

  temp = (dat & 0x0f) << 4;
  for (i = DB[0]; i <= 10; i++) {
    digitalWrite(i, temp & 0x80);
    temp <<= 1;
  }

  digitalWrite(LCD1602_EN, HIGH);
  delayMicroseconds(1);
  digitalWrite(LCD1602_EN, LOW);
}

void LCD_SET_XY(int x, int y) {
  int address;
  if (y == 0) address = 0x80 + x;
  else address = 0xC0 + x;
  LCD_Command_Write(address);
}

void LCD_Write_Char(int x, int y, int dat) {
  LCD_SET_XY(x, y);
  LCD_Data_Write(dat);
}

void LCD_Write_String(int X, int Y, char *s) {
  LCD_SET_XY(X, Y);
  //设置地址
  while (*s)  //写字符串
  {
    LCD_Data_Write(*s);

    s++;
  }
}

DFRobot_DHT11 DHT;

void setup() {
  Serial.begin(9600);            //UART setup, baudrate = 9600bps
  pinMode(BOOL_PIN, INPUT);      //set pin to input
  digitalWrite(BOOL_PIN, HIGH);  //turn on pullup resistors

  int i = 0;
  for (i = 6; i <= 12; i++) {
    pinMode(i, OUTPUT);
  }

  delay(100);
  LCD_Command_Write(0x28);  //4线 2行 5x7
  delay(50);
  LCD_Command_Write(0x06);
  delay(50);
  LCD_Command_Write(0x0c);
  delay(50);
  LCD_Command_Write(0x80);
  delay(50);
  LCD_Command_Write(0x01);
  delay(50);

  LCD_Write_String(0, 0, "                ");
  LCD_Write_String(0, 1, "                ");

  char title1[] = "TVOC & Humidity";
  char title2[] = "Temperature";
  LCD_Write_String((int)((16 - strlen(title1)) / 2), 0, title1);  //第0行，第4个地址起
  LCD_Write_String((int)((16 - strlen(title2)) / 2), 1, title2);  //第1行，第4个地址起

  delay(1000);

  char title3[] = "Loading...";
  LCD_Write_String(0, 0, "                ");
  LCD_Write_String(0, 1, "                ");
  LCD_Write_String((int)((16 - strlen(title3)) / 2), 0, title3);  //第0行，第4个地址起

  delay(1000);

  delay(100);
  LCD_Command_Write(0x28);  //4线 2行 5x7
  delay(50);
  LCD_Command_Write(0x06);
  delay(50);
  LCD_Command_Write(0x0c);
  delay(50);
  LCD_Command_Write(0x80);
  delay(50);
  LCD_Command_Write(0x01);
  delay(50);
  
  delay(1000);
}

// int refresh = 3;

void loop() {
  // // 刷新一段时间
  // if (refresh > 0) {
  //   delay(100);
  //   LCD_Command_Write(0x28);  //4线 2行 5x7
  //   delay(50);
  //   LCD_Command_Write(0x06);
  //   delay(50);
  //   LCD_Command_Write(0x0c);
  //   delay(50);
  //   LCD_Command_Write(0x80);
  //   delay(50);
  //   LCD_Command_Write(0x01);
  //   delay(50);

  //   refresh--;

  //   return;
  // }

  float volts = MGRead(MG_PIN);

  int r = (5 * 3900) / volts - 3900;

  int percentage = 0;

  // 预热
  if (n < READ_WARMUP) {
    RS_AIR_V_MIN = volts;
    RS_AIR_R_MAX = r;
  }

  percentage = MGGetPercentage(volts);
  percentage = 100 - percentage;

  char line1[16] = "";
  strcat(line1, "TVOC:");
  char tvocValue[4] = "";
  itoa(percentage, tvocValue, 10);
  if (n < READ_WARMUP) {
    strcat(line1, "Wait");
  } else {
    strcat(line1, tvocValue);
    strcat(line1, "%");
  }

  LCD_Write_String(0, 0, "                ");
  LCD_Write_String(0, 0, line1);

  char vValue[5] = "";
  itoa(r, vValue, 10);
  LCD_Write_String(16 - strlen(vValue), 0, vValue);

  DHT.read(2);

  char line2[16] = "";
  char tValue[4] = "";
  char hValue[4] = "";
  itoa(DHT.temperature, tValue, 10);
  itoa(DHT.humidity, hValue, 10);
  strcat(line2, "Temp:");
  strcat(line2, tValue);
  strcat(line2, "C Hum:");
  strcat(line2, hValue);
  strcat(line2, "%");

  LCD_Write_String(0, 1, "                ");
  LCD_Write_String(0, 1, line2);

  Serial.println("");
  Serial.print(n);
  Serial.println("秒");
  Serial.print("温度：");
  Serial.print(DHT.temperature);
  Serial.println("°C");
  Serial.print("湿度：");
  Serial.print(DHT.humidity);
  Serial.println("%");
  Serial.print("TVOC：");

  Serial.print(percentage);
  Serial.print("%（");
  Serial.print(r);
  Serial.print("Ω/");
  Serial.print(RS_AIR_R_MAX);
  if (n < READ_WARMUP) {
    Serial.print("Ω）");
    Serial.print("（预热中）");
  } else {
    Serial.println("Ω）");
  }

  // Serial.println(n);

  // delay(100);
  // LCD_Command_Write(0x28);  //4线 2行 5x7
  // delay(50);
  // LCD_Command_Write(0x06);
  // delay(50);
  // LCD_Command_Write(0x0c);
  // delay(50);
  // LCD_Command_Write(0x80);
  // delay(50);
  // LCD_Command_Write(0x01);
  // delay(50);

  // char test[4] = "";
  // itoa(n, test, 10);
  // LCD_Write_String(0, 1, "                ");
  // LCD_Write_String((int)((16 - strlen(test)) / 2), 1, test);
  n++;

  delay(1000);
}

float MGRead(int mg_pin) {
  int i;
  float v = 0;

  for (i = 0; i < READ_SAMPLE_TIMES; i++) {
    v += analogRead(mg_pin);
    delay(READ_SAMPLE_INTERVAL);
  }
  v = (v / READ_SAMPLE_TIMES) * 5 / 1024;  //单位是V
  return v;
}

int MGGetPercentage(float volts) {
  RS_GAS_R = (5 * 3900) / volts - 3900;  //计算气体中的阻值RS_GAS_R
  return RS_GAS_R / RS_AIR_R_MAX * 100;
}