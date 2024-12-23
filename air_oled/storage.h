#include <Arduino.h>

// SD 卡
#include <SPI.h>
#include <SD.h>

// SD 卡
// SPI接口：MOSI->D11、MISO->D12、SCK->D13
// 接口：VCC->VCC(5V)、GND->GND、CS->D10
// 协议：SPI

namespace Storage {
Sd2Card _card;
SdVolume _volume;

struct _SD_Card {
  bool Init(int pin, char* error) {
    SD.begin();

    strcpy(error, "");

    if (!_card.init(SPI_HALF_SPEED, pin) || !_volume.init(_card)) {
      strcpy(error, "SD failed.");
      Serial.println(error);

      return false;
    } else {
      return true;
    }

    return false;
  }

  void GetFileName(DateTime& now, char* filename) {
    int year = now.year();
    int month = now.month();
    int day = now.day();
    int hour = now.hour();
    int minute = now.minute();
    int second = now.second();

    char yearStr[] = "0000";
    itoa(year, yearStr, 10);

    char monthStr[] = "00";
    itoa(month, monthStr, 10);
    if (month < 10) {
      monthStr[1] = monthStr[0];
      monthStr[0] = '0';
    }

    char dayStr[] = "00";
    itoa(day, dayStr, 10);
    if (day < 10) {
      dayStr[1] = dayStr[0];
      dayStr[0] = '0';
    }

    char hourStr[] = "00";
    itoa(hour, hourStr, 10);
    if (hour < 10) {
      hourStr[1] = hourStr[0];
      hourStr[0] = '0';
    }

    char minuteStr[] = "00";
    itoa(minute, minuteStr, 10);
    if (minute < 10) {
      minuteStr[1] = minuteStr[0];
      minuteStr[0] = '0';
    }

    char secondStr[] = "00";
    itoa(second, secondStr, 10);
    if (second < 10) {
      secondStr[1] = secondStr[0];
      secondStr[0] = '0';
    }

    // Serial.println(year);
    // Serial.println(month);
    // Serial.println(day);
    // Serial.println(hour);
    // Serial.println(minute);
    // Serial.println(second);

    // Serial.println(yearStr);
    // Serial.println(monthStr);
    // Serial.println(dayStr);
    // Serial.println(hourStr);
    // Serial.println(minuteStr);
    // Serial.println(secondStr);

    // 20241219.TXT
    strcat(filename, yearStr);
    strcat(filename, monthStr);
    strcat(filename, dayStr);
    strcat(filename, ".TXT");

    Serial.println(filename);
  }

  bool WriteLog(DateTime& now) {
    int year = now.year();
    int month = now.month();
    int day = now.day();
    int hour = now.hour();
    int minute = now.minute();
    int second = now.second();

    char yearStr[] = "0000";
    itoa(year, yearStr, 10);

    char monthStr[] = "00";
    itoa(month, monthStr, 10);
    if (month < 10) {
      monthStr[1] = monthStr[0];
      monthStr[0] = '0';
    }

    char dayStr[] = "00";
    itoa(day, dayStr, 10);
    if (day < 10) {
      dayStr[1] = dayStr[0];
      dayStr[0] = '0';
    }

    char hourStr[] = "00";
    itoa(hour, hourStr, 10);
    if (hour < 10) {
      hourStr[1] = hourStr[0];
      hourStr[0] = '0';
    }

    char minuteStr[] = "00";
    itoa(minute, minuteStr, 10);
    if (minute < 10) {
      minuteStr[1] = minuteStr[0];
      minuteStr[0] = '0';
    }

    char secondStr[] = "00";
    itoa(second, secondStr, 10);
    if (second < 10) {
      secondStr[1] = secondStr[0];
      secondStr[0] = '0';
    }

    // Serial.println(year);
    // Serial.println(month);
    // Serial.println(day);
    // Serial.println(hour);
    // Serial.println(minute);
    // Serial.println(second);

    // Serial.println(yearStr);
    // Serial.println(monthStr);
    // Serial.println(dayStr);
    // Serial.println(hourStr);
    // Serial.println(minuteStr);
    // Serial.println(secondStr);

    // 20241219.TXT
    char* filename = (char*)malloc(1 * sizeof(char));
    filename[0] = '\0';

    strcat(filename, yearStr);
    strcat(filename, monthStr);
    strcat(filename, dayStr);
    strcat(filename, ".TXT");

    Serial.println(filename);

    File writeFile = SD.open(filename, FILE_WRITE);

    if (writeFile) {
      char* record = (char*)malloc(1 * sizeof(char));
      record[0] = '\0';

      strcat(record, "[Time:");
      strcat(record, yearStr);
      strcat(record, monthStr);
      strcat(record, dayStr);
      strcat(record, hourStr);
      strcat(record, minuteStr);
      strcat(record, secondStr);
      strcat(record, "][HCHO:");
      strcat(record, "2.0");
      strcat(record, "][TVOC:");
      strcat(record, "2.0");
      strcat(record, "][CO2:");
      strcat(record, "2.0");
      strcat(record, "][Temp:");
      strcat(record, "2.0");
      strcat(record, "][Hum:");
      strcat(record, "2.0");
      strcat(record, "][UV:");
      strcat(record, "2.0");
      strcat(record, "][PM1:");
      strcat(record, "2.0");
      strcat(record, "][PM2.5:");
      strcat(record, "2.0");
      strcat(record, "][PM10:");
      strcat(record, "2.0");
      strcat(record, "][BARO:");
      strcat(record, "2. 0");
      strcat(record, "]");

      Serial.println(record);
      Serial.println(strlen(record));

      writeFile.println(record);

      while (writeFile.available()) {
        Serial.write(writeFile.read());
      }

      writeFile.close();

      free(record);
    }

    free(filename);
  }

  void ReadLog(DateTime& now) {
    char* filename = (char*)malloc(1 * sizeof(char));
    filename[0] = '\0';
    GetFileName(now, filename);

    Serial.println("filename:");
    Serial.println(filename);
    if (SD.exists(filename)) {
      Serial.println("exists");
      File logFile = SD.open(filename);
      if (logFile) {
        // while (logFile.available()) {
        //   Serial.write(logFile.read());
        // }
        // logFile.seek(1);
        // String lista = logFile.readStringUntil('\r');
        // Serial.println(lista);

        // String Sub_String_A = "";
        // String Sub_String_B = "";
        // Sub_String_A = S.separa(line, '|', 0);
        // Sub_String_B = S.separa(line, '|', 1);
        // Serial.println(Sub_String_A);
        // Serial.println(Sub_String_B);

        String line = "";
        // char line2[] = "";

        int pos = 0;
        int total = logFile.size();

        while (logFile.available()) {
          for (pos = 0; pos <= total; pos++) {
            char c = logFile.read();
            Serial.println(strlen(line.c_str()));
            // Serial.println(strlen(line2));
            if (c == '\n') {
              Serial.println(line);
              // Serial.println(line2);
              line = "";
              // strcpy(line2, "");
            } else if (c != '\r') {
              line = line + c;
              // char cc[] = { c };
              // strcat(line2, cc);
            }
            delay(100);
          }
        }
      }
    }
  }

  // float percent = 0;
  // float kb = 0;
  // float mb = 0;
  // float gb = 0;

  // unsigned long Search(File dir, unsigned long total, int numTabs) {
  //   while (true) {
  //     File file = dir.openNextFile();
  //     if (!file) {
  //       file.close();
  //       break;
  //     }
  //     for (uint8_t i = 0; i < numTabs; i++) {
  //       Serial.print('\t');
  //     }
  //     Serial.print(file.name());
  //     if (file.isDirectory()) {
  //       Serial.println("/");
  //       total += Search(file, 0, numTabs + 1);
  //     } else {
  //       Serial.print("\t\t");
  //       Serial.println(file.size(), DEC);
  //       total += file.size();
  //     }
  //     file.close();
  //   }

  //   Serial.print("total:");
  //   Serial.println(total);

  //   return total;
  // }

  // unsigned long CalcUsed() {
  //   unsigned long usage = 0;

  //   File home = SD.open("/");

  //   usage = Search(home, 0, 0);

  //   Serial.print("usage:");
  //   Serial.println(usage);

  //   home.close();

  //   return usage;
  // }

  // float CalcPercent() {
  //   kb = volume.blocksPerCluster();  // clusters are collections of blocks
  //   kb *= volume.clusterCount();     // we'll have a lot of clusters
  //   kb /= 2.0;                       // SD card blocks are always 512 bytes (2 blocks are 1 KB)

  //   mb = kb / 1024.0;  // MB
  //   gb = mb / 1024.0;

  //   Serial.print("SD:");
  //   Serial.print(kb);
  //   Serial.print("KB/");
  //   Serial.print(mb);
  //   Serial.print("MB/");
  //   Serial.print(gb);
  //   Serial.println("GB");

  //   unsigned long used = CalcUsed();

  //   used = used / 1024;

  //   unsigned long remaining = kb - used;

  //   Serial.print("used:");
  //   Serial.println(used);

  //   Serial.print("remaining:");
  //   Serial.println(remaining);

  //   percent = (float)remaining / (float)kb * (float)100;

  //   return percent;
  // }
} SD_Card;

}