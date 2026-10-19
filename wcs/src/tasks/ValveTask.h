#ifndef __VALVE_TASK__
#define __VALVE_TASK__

#include "kernel/Task.h"
#include "model/Context.h"
#include "devices/ServoMotor.h"
#include <Arduino.h>

class ValveTask: public Task {

public:
  ValveTask(ServoMotor* pValve, Context* pContext); 
  void tick();

private:  
  void setState(int state);
  long elapsedTimeInState();
  void log(const String& msg);
  
  bool checkAndSetJustEntered();
  
  enum TaskStates { IDLE, MOVING } state;
  long stateTimestamp;
  bool justEntered;

  ServoMotor* pValve;
  Context* pContext;
};

#endif