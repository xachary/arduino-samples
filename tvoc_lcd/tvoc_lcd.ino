#include "lcd_1602_4.h"  // LCD显示
#include "dht_11.h"          // 温湿度
#include "kq_2801.h"         // TVOC指数

// 工具实例

// 【四位接法】
// 显示屏背面：1->GND、2->VCC(3.3V)、3->D6、4->D7、5->D8、6->D9
// 显示屏正面：1->GND、2->VCC(5V)、3->电阻110Ω->GND、4->D12、5->D11、6->D10、7->D10
int DB[4] = { 6, 7, 8, 9 };
LCD_1602_4 LCD(12, 11, 10, DB);

// 镂空面：1->VCC(5V)、2->D?、3->留空、4->GND
DHT_11 DHT(13);

// 元器件面：1(AO)->A、2(DO)->D?、3(GND)->GND、4(VCC)->VCC(5V)
KQ_2801 KQ(A0, 5);

// 元器件面（针脚在下方）：1->VCC(5V)、2->A?、3->GND
HC_SR04 SR04(5, 4);

// 元器件面（针脚在上方）：1->VCC(5V)、2->D?(~)、3->D?、4->GND
HC_SR501 SR501(A1);

// 正(+)->D?、负(-)->GND
Buzzer_Active BA(3);

// 正(长)->D?、负(短)->GND
Buzzer_Passive BP(A3);

void setup() {
  Serial.begin(9600);  //UART setup, baudrate = 9600bps

  // 工具初始化
  LCD.Init();
  KQ.Init();
  // SR501.Init();
  // BA.Init();
  // BP.Init();

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

  delay(1000);

  // 清屏
  LCD.Clear();
}

// 执行次数
long n = 0;
// 帧计数
long timer = 0;

// 执行逻辑
void process(char distanceStr[]) {
  if (n < 5) {
    LCD.Init();
  }

  // 读数
  DHT.Read();
  KQ.Read();

  // LCD显示内容

  char line1[16] = "";
  strcat(line1, "TVOC:");
  char percentageStr[4] = "";
  itoa(KQ.percentage, percentageStr, 10);
  strcat(line1, percentageStr);
  strcat(line1, "%");
  if (KQ.is_warm_up) {
    strcat(line1, "W");
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
  // SR04.Read();
  // SR501.Read();

  // 距离小于20cm，有源蜂鸣器响一下
  // if (SR04.distance < 20) {
  //   BP.Sound(100, 2, 6);
  // }

  // 有源蜂鸣器：重置
  // BA.Off();

  // LCD显示内容
  char distanceStr[5] = "";
  // itoa(SR04.distance, distanceStr, 10);
  // strcat(distanceStr, "cm");

  // 每秒（大约）执行一次
  if (timer > 20 * 500) {
    // 重新计数
    timer = 0;
    // 累计执行次数
    n++;
    // 有人/无人
    // if (SR501.active) {
    //   // 有人，补充LCD显示内容
    //   char activeStr[6] = "P";
    //   strcat(activeStr, distanceStr);

    //   process(activeStr);

    //   // 有源蜂鸣器：响一下
    //   BA.On();
    // } else {
    //   process(distanceStr);
    // }
    process(distanceStr);
  }
}