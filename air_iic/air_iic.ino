#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "dht_11.h"   // 温湿度
#include "kq_2801.h"  // TVOC指数

// 工具实例

// 遇到空屏，可能需要调节iis版的电阻旋钮（先试试顺时针，调45°）。
// VCC->VCC(5V/3.3V)、GND->GND、SCL->SCL/A4、SDA->SDA/A5
LiquidCrystal_I2C LCD(0x27, 16, 2);

// 镂空面：1->VCC(5V)、2->D?、3->留空、4->GND
DHT_11 DHT(12);

// 元器件面：1(AO)->A、2(DO)->D?、3(GND)->GND、4(VCC)->VCC(5V)
KQ_2801 KQ(A0, 5);

int btnPin = 2;

void setup() {
  Serial.begin(9600);  //UART setup, baudrate = 9600bps

  // 关掉主板的灯L
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  pinMode(btnPin, INPUT);

  // 工具初始化
  KQ.Init();

  LCD.init();
  LCD.backlight();

  // 加载提醒
  char title1[] = "Air Monitor";
  LCD.setCursor((int)((16 - strlen(title1)) / 2), 0);
  LCD.print(title1);
  char title2[] = "Loading...";
  LCD.setCursor((int)((16 - strlen(title2)) / 2), 1);
  LCD.print(title2);

  Serial.println("");
  Serial.println("Loading...");
}

// 执行次数
long n = 0;
// 阈值
long at = 100000;
// 帧计数
long timer = at;

void printTVOC() {
  char line[16] = "";

  strcat(line, "TVOC:");
  char percentageStr[4] = "";
  itoa(KQ.percentage, percentageStr, 10);
  strcat(line, percentageStr);
  strcat(line, "%");

  if (KQ.is_warm_up) {
    char subfix[] = "";
    itoa(KQ.warm_up_seconds - KQ.read_times, subfix, 10);
    for (int i = strlen(line); i < 16 - strlen(subfix); i++) {
      strcat(line, " ");
    }
    strcat(line, subfix);
  }

  char display[16] = "                ";
  int i = 0;
  for (; i < strlen(line); i++) {
    display[i] = line[i];
  }

  LCD.setCursor(0, 0);
  LCD.print(display);

  display[i] = '\0';
  Serial.println(display);
}

void printTempHum() {
  char line[16] = "";

  char temperatureStr[4] = "";
  char humidityStr[4] = "";

  itoa(DHT.temperature, temperatureStr, 10);
  itoa(DHT.humidity, humidityStr, 10);
  strcat(line, "Tem:");
  strcat(line, temperatureStr);
  strcat(line, " C");

  // 左右贴边，补充空格
  for (int i = 0; i < 16 - (4 + strlen(temperatureStr) + 2) - (4 + strlen(humidityStr) + 1); i++) {
    strcat(line, " ");
  }

  strcat(line, "Hum:");
  strcat(line, humidityStr);
  strcat(line, "%");

  int pos = strlen(temperatureStr) + 4;
  line[pos] = 0xdf;

  char display[16] = "                ";
  int i = 0;
  for (; i < strlen(line); i++) {
    display[i] = line[i];
  }

  LCD.setCursor(0, 1);
  LCD.print(display);

  display[i] = '\0';
  Serial.println(display);
}

// 执行逻辑
void process() {
  // 读数
  DHT.Read();
  KQ.Read();

  Serial.println("");
  Serial.print(n);
  Serial.println("秒");

  // LCD显示内容
  printTVOC();
  printTempHum();

  // 串口日志
  Serial.print(KQ.resistance_current);
  Serial.print("Ω/");
  Serial.print(KQ.resistance_air);
  if (KQ.is_warm_up) {
    Serial.print("Ω");
    Serial.println("（预热中）");
  } else {
    Serial.println("Ω");
  }
}

bool lightOn = true;
bool btnClick = false;

// 帧
void loop() {
  // Click 判断
  int btnOn = digitalRead(btnPin);
  if (!btnClick && btnOn == HIGH) {
    btnClick = true;
    // 切换状态
    lightOn = !lightOn;

    // 开关灯
    if (lightOn) {
      LCD.backlight();
    } else {
      LCD.noBacklight();
    }
  } else if (btnOn == LOW) {
    btnClick = false;
  }

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