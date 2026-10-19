#ifndef __INTERFACE_TASK__
#define __INTERFACE_TASK__

#include "kernel/Task.h"
#include "model/Context.h"
#include "kernel/MsgService.h"
#include <Arduino.h>

class InterfaceTask: public Task {

public:
  InterfaceTask(Context* pContext); 
  void tick();

private:  
  void setState(int state);
  long elapsedTimeInState();
  
  bool checkAndSetJustEntered();
  
  void parseSerial();
  String readSerial();
  bool processData(String data);
  bool parseState(const String& s, Context::ControllerStates& out);
  void updateCUS();

  enum TaskStates { COMM } state;
  long stateTimestamp;
  bool justEntered;

  Context* pContext;
};

#endif