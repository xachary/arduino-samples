#include "lcd_1602_4.h"  // LCD显示
#include "dht_11.h"      // 温湿度
#include "kq_2801.h"     // TVOC指数
#include "hc_sr04.h"     // 超声波距离
#include "hc_sr501.h"    // 红外人体感应

// 工具实例

int DB[4] = { 6, 7, 8, 9 };
LCD_1602_4 LCD(12, 11, 10, DB);

DHT_11 DHT(2);

KQ_2801 KQ(A0, 13);

HC_SR04 SR04(5, 4);

HC_SR501 SR501(A1);

void setup() {
  Serial.begin(9600);  //UART setup, baudrate = 9600bps

  // 工具初始化
  LCD.Init();
  KQ.Init();
  SR501.Init();

  delay(1000);

  // 欢迎语
  char title1[] = "TVOC & Humidity";
  char title2[] = "Temperature";
  LCD.Write_String((int)((16 - strlen(title1)) / 2), 0, title1);  //第0行，第4个地址起
  LCD.Write_String((int)((16 - strlen(title2)) / 2), 1, title2);  //第1行，第4个地址起

  delay(1000);

  // 加载提醒
  char title3[] = "Loading...";
  LCD.Clear();
  LCD.Write_String((int)((16 - strlen(title3)) / 2), 0, title3);  //第0行，第4个地址起

  // 清屏
  LCD.Clear();

  delay(100);
}

// 执行次数
long n = 0;
// 帧计数
long timer = 0;

// 执行逻辑
void process(char distanceStr[]) {
  // 读数
  DHT.Read();
  KQ.Read();

  // LCD显示内容

  char line1[16] = "";
  strcat(line1, "TVOC:");
  char percentageStr[4] = "";
  itoa(KQ.percentage, percentageStr, 10);
  if (KQ.is_warm_up) {
    strcat(line1, "Wait");
  } else {
    strcat(line1, percentageStr);
    strcat(line1, "%");
  }

  LCD.Clear_Line(0);
  LCD.Write_String(0, 0, line1);

  LCD.Write_String(16 - strlen(distanceStr), 0, distanceStr);

  char line2[16] = "";
  char temperatureStr[4] = "";
  char humidityStr[4] = "";
  itoa(DHT.temperature, temperatureStr, 10);
  itoa(DHT.humidity, humidityStr, 10);
  strcat(line2, "Temp:");
  strcat(line2, temperatureStr);
  strcat(line2, "C Hum:");
  strcat(line2, humidityStr);
  strcat(line2, "%");

  LCD.Clear_Line(1);
  LCD.Write_String(0, 1, line2);

  // 串口日志

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
  Serial.print(KQ.percentage);
  Serial.print("%（");
  Serial.print(KQ.resistance_current);
  Serial.print("Ω/");
  Serial.print(KQ.resistance_air);
  if (KQ.is_warm_up) {
    Serial.print("Ω）");
    Serial.println("（预热中）");
  } else {
    Serial.println("Ω）");
  }
  Serial.print("距离：");
  Serial.println(distanceStr);
  if (SR501.active) {
    Serial.print("有人出现：");
    Serial.println(SR501.value);
  } else {
    Serial.print("无人出现：");
    Serial.println(SR501.value);
  }
}

// 帧
void loop() {
  // 计数
  timer++;

  // 读数
  SR04.Read();
  SR501.Read();

  // LCD显示内容
  char distanceStr[5] = "";
  itoa(SR04.distance, distanceStr, 10);
  strcat(distanceStr, "cm");

  // 每秒（大约）执行一次
  if (timer > 20) {
    // 重新计数
    timer = 0;
    // 累计执行次数
    n++;
    // 有人/无人
    if (SR501.active) {
      // 有人，补充LCD显示内容
      char activeStr[6] = "P";
      strcat(activeStr, distanceStr);

      process(activeStr);
    } else {
      process(distanceStr);
    }
  }
}