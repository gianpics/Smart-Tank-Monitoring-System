#include "HWPlatform.h"
#include <Arduino.h>
#include "devices/ButtonImpl.h"
#include "kernel/MsgService.h"
#include "devices/ServoMotorImpl.h"
#include "config.h"
#include "kernel/Logger.h"

void wakeUp(){}

HWPlatform::HWPlatform(){
  pButton = new ButtonImpl(BTN_PIN);
  pValve = new ServoMotorImpl(MOTOR_PIN);
  pLcd = new Lcd(LCD_PORT, LCD_COLS_NUM, LCD_ROWS_NUM);
  pPot = new Potentiometer(POT_PIN);
}


void HWPlatform::init(){
  Logger.log("Test WCS...");
  pValve->on();
  pValve->setPosition(0);
  pValve->off();
}

Button* HWPlatform::getButton(){
  return this->pButton;
}

ServoMotor* HWPlatform::getValve(){
  return this->pValve;
}

Lcd* HWPlatform::getLcd(){
  return this->pLcd;
}

Potentiometer *HWPlatform::getPotentiometer()
{
  return this->pPot;
}

void HWPlatform::test(){
  Logger.log("==========================");
  
  bool btPressed = pButton->isPressed();
  pPot->sync();
  float potValue = pPot->getValue();
  float potDegs = potValue * VALVE_OPENING_DEG;


  pValve->on();
  pValve->setPosition(VALVE_OPENING_DEG);
  pLcd->display(LCD_TEST_STRING, 1, 1);

  Logger.log("Button: " + String(btPressed ? "pressed" : " not pressed"));
  Logger.log("Pot value: " + String(potValue));
  Logger.log("Corresponding to valve: " + String(potDegs) + "°");
  delay(1000);
  pValve->setPosition(0);
  delay(SERVO_ADJUST_TIME);
  pValve->off();
  
}
