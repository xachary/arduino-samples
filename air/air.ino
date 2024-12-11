#include "lcd_1602_iic.h"  // LCD显示
#include "dht_11.h"      // 温湿度
#include "kq_2801.h"     // TVOC指数

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

void setup() {
  Serial.begin(9600);  //UART setup, baudrate = 9600bps

  // 工具初始化
  KQ.Init();

  delay(1000);
}

void loading() {
  LCD.Init();
  // 加载提醒
  char title3[] = "Loading...";
  LCD.Write_String((int)((16 - strlen(title3)) / 2), 0, title3);  //第0行，第4个地址起

  Serial.println("Loading...");
}

// 执行次数
long n = 0;
// 阈值
long at = 500000;
// 帧计数
long timer = at;

// 执行逻辑
void process() {
  if (n <= 2) {
    loading();
  } else {
    // 读数
    DHT.Read();
    KQ.Read();

    // LCD显示内容

    char line1[] = "";
    strcat(line1, "TVOC:");
    char percentageStr[4] = "";
    itoa(KQ.percentage, percentageStr, 10);
    strcat(line1, percentageStr);
    strcat(line1, "%");
    if (KQ.is_warm_up) {
      strcat(line1, "W");
    }

    for (int i = strlen(line1); i < 16; i++) {
      strcat(line1, " ");
    }

    LCD.Write_String(0, 0, line1);

    char line2[] = "";
    char temperatureStr[4] = "";
    char humidityStr[4] = "";
    itoa(DHT.temperature, temperatureStr, 10);
    itoa(DHT.humidity, humidityStr, 10);
    strcat(line2, "Temp:");
    strcat(line2, temperatureStr);
    strcat(line2, "C Hum:");
    strcat(line2, humidityStr);
    strcat(line2, "%");

    for (int i = strlen(line2); i < 16; i++) {
      strcat(line2, " ");
    }
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
  }
}

// 帧
void loop() {
  // 计数
  timer++;

  // 每秒（大约）执行一次
  if (timer >= at) {
    // 重新计数
    timer = 0;
    // 累计执行次数
    n++;
    // 执行
    process();
  }
}