#ifndef __HW_PLATFORM__
#define __HW_PLATFORM__

#include "config.h"
#include "devices/Button.h"
#include "devices/Lcd.h"
#include "devices/ServoMotor.h"
#include "devices/Pot.h"

class HWPlatform {

public:
  HWPlatform();
  void init();
  void test();

  Button* getButton();
  ServoMotor* getValve();
  Lcd* getLcd();
  Potentiometer* getPotentiometer();

private:
  Button* pButton;
  ServoMotor* pValve;
  Lcd* pLcd;
  Potentiometer* pPot;
};

#endif
