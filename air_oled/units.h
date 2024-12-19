
namespace Units {

const int ModeLength = 11;

enum Mode {
  Home = 0,
  HCHO = 1,
  TVOC = 2,
  CO2 = 3,
  Temp = 4,
  Hum = 5,
  BARO = 6,
  UV = 7,
  PM1 = 8,
  PM2_5 = 9,
  PM10 = 10,
};

void getUnit(Mode m, char* ret) {
  switch (m) {
    case HCHO: strcpy(ret, "ug\/m"); break;
    case TVOC: strcpy(ret, "ppb"); break;
    case CO2: strcpy(ret, "ppm"); break;
    case Temp: strcpy(ret, " C"); break;
    case Hum: strcpy(ret, "%"); break;
    case BARO: strcpy(ret, "KPa"); break;
    case UV: strcpy(ret, "index"); break;
    case PM1: strcpy(ret, "ug\/m"); break;
    case PM2_5: strcpy(ret, "ug\/m"); break;
    case PM10: strcpy(ret, "ug\/m"); break;
    default: strcpy(ret, "");
  }
}

void getModeName(Mode m, char* ret) {
  switch (m) {
    case HCHO: strcpy(ret, "HCHO"); break;
    case TVOC: strcpy(ret, "TVOC"); break;
    case CO2: strcpy(ret, "CO2"); break;
    case Temp: strcpy(ret, "Temp"); break;
    case Hum: strcpy(ret, "Hum"); break;
    case BARO: strcpy(ret, "BARO"); break;
    case UV: strcpy(ret, "UV"); break;
    case PM1: strcpy(ret, "PM1"); break;
    case PM2_5: strcpy(ret, "PM2.5"); break;
    case PM10: strcpy(ret, "PM10"); break;
    default: strcpy(ret, "Home");
  }
}

}