#include "mtp_40_f.h"
//
//    FILE: MTP40F_PWM_demo.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: demo of MTP40F PWM interface
//     URL: https://github.com/RobTillaart/MTP40F
//
//  TODO TEST WITH MTP40F SENSOR
//
//  Connect the PWM output to the interrupt pin 2 or 3 of the UNO.
//  other processors interrupts pins work slightly different

// https://github.com/RobTillaart/MTP40F

MTP_40_F mtp_40_f(3);

void setup() {
  Serial.begin(9600);

  mtp_40_f.Init();
}


void loop() {
  mtp_40_f.Read();

  Serial.println(mtp_40_f.value);

  delay(500);
}