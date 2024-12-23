
namespace Modes {

const unsigned int Length = 10;

enum Mode {
  Home = 0,
  HCHO = 1,
  TVOC = 2,
  CO2 = 3,
  Temp = 4,
  Hum = 5,
  UV = 6,
  PM1 = 7,
  PM2_5 = 8,
  PM10 = 9,
};

void getUnit(Mode m, char* ret) {
  switch (m) {
    case HCHO: strcpy(ret, "HCHO"); break;
    case TVOC: strcpy(ret, "TVOC"); break;
    case CO2: strcpy(ret, "CO2"); break;
    case Temp: strcpy(ret, "Temp"); break;
    case Hum: strcpy(ret, "Hum"); break;
    case UV: strcpy(ret, "UV"); break;
    case PM1: strcpy(ret, "PM1"); break;
    case PM2_5: strcpy(ret, "PM2.5"); break;
    case PM10: strcpy(ret, "PM10"); break;
    default: strcpy(ret, "Home");
  }
}

void getModeName(Mode m, char* ret) {
  switch (m) {
    case HCHO: strcpy(ret, "HCHO"); break;
    case TVOC: strcpy(ret, "TVOC"); break;
    case CO2: strcpy(ret, "CO2"); break;
    case Temp: strcpy(ret, "Temp"); break;
    case Hum: strcpy(ret, "Hum"); break;
    case UV: strcpy(ret, "UV"); break;
    case PM1: strcpy(ret, "PM1"); break;
    case PM2_5: strcpy(ret, "PM2.5"); break;
    case PM10: strcpy(ret, "PM10"); break;
    default: strcpy(ret, "Home");
  }
}

}