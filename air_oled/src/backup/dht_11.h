#include <Arduino.h>
#include <DFRobot_DHT11.h>

class DHT_11 {
public:
  DHT_11(int p) {
    pin = p;
  }

  int pin = 0;
  int temperature = 0;
  int humidity = 0;
  DFRobot_DHT11 DHT11;

  void Read() {
    DHT11.read(pin);
    temperature = DHT11.temperature;
    humidity = DHT11.humidity;
  }
};