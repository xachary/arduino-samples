#include <Arduino.h>

#define _Pin_Btn_1 11
#define _Pin_Btn_2 12

namespace Buttons {

enum Status {
  Ready = 0,
  Down = 1,
  Up = 2
};

struct _Btn_1 {
  Status status = Ready;
  
  void loop() {
    if (status == Ready && digitalRead(_Pin_Btn_1) == LOW) {
      status = Down;
    }

    if (status != Ready && digitalRead(_Pin_Btn_1) == HIGH) {
      status = Up;
    }
  }

  bool getValue(){
    bool result = status == Up;
    status = Ready;
    return result;
  }
} Btn_1;

struct _Btn_2 {
  Status status = Ready;
  
  void loop() {
    if (status == Ready && digitalRead(_Pin_Btn_2) == LOW) {
      status = Down;
    }

    if (status != Ready && digitalRead(_Pin_Btn_2) == HIGH) {
      status = Up;
    }
  }

  bool getValue(){
    bool result = status == Up;
    status = Ready;
    return result;
  }
} Btn_2;

}