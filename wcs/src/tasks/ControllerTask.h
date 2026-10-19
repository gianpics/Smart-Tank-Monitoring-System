#ifndef __CONTROLLER_TASK__
#define __CONTROLLER_TASK__

#include "kernel/Task.h"
#include "devices/Button.h"
#include "devices/Lcd.h"
#include "devices/Pot.h"
#include "model/Context.h"
#include "kernel/MsgService.h"
#include <Arduino.h>

class ControllerTask: public Task {

public:
  ControllerTask(Button* pButton, Lcd* pLcd, Potentiometer* pPot, Context* pContext); 
  void tick();
  void resetTask();

private:  
  void setState(int state);
  long elapsedTimeInState();
  void log(const String& msg);
  bool checkAndSetJustEntered();
  
  void updateLCD();
  bool isManualRequested();
  bool isAutoRequested();
  bool pressDebounce();
  void resetPreviousControlsValues();
  void updateValveWithRemotePosition();
  int getPotentiometerValue();

  int prevRemoteVal;
  int prevPotValue;

  enum Context::ControllerStates state;
  enum Context::ControllerStates previousState;
  long safeDistanceTimestamp;
  long stateTimestamp;
  bool justEntered;

  Button* pButton;
  Lcd* pLcd;
  Potentiometer* pPot;
  Context* pContext;
};

#endif