#include <Arduino.h>
#include "config.h"
#include "kernel/Scheduler.h"
#include "kernel/Logger.h"
#include "kernel/MsgService.h"
#include "model/HWPlatform.h"
#include "tasks/TestHWTask.h"
#include "tasks/ControllerTask.h"
#include "tasks/ValveTask.h"
#include "tasks/InterfaceTask.h"

//#define __TESTING_HW__

Scheduler sched;

HWPlatform* pHWPlatform;
Context* pContext;

void setup() {
  MsgService.init();
  sched.init(50);

  Logger.log(".::::: Water Channel Subsystem :::::.");
  
  pHWPlatform = new HWPlatform();
  pHWPlatform->init();

#ifndef __TESTING_HW__
  pContext = new Context();

  // Interface
  Task* pInterfaceTask = new InterfaceTask(pContext);
  pInterfaceTask->init(100);
  
  // Controller
  Task* pControllerTask = new ControllerTask(pHWPlatform->getButton(), pHWPlatform->getLcd(), pHWPlatform->getPotentiometer(), pContext);
  pControllerTask->init(50); 

  // Valve
  Task* pValveTask = new ValveTask(pHWPlatform->getValve(), pContext);
  pValveTask->init(50);
  
  sched.addTask(pInterfaceTask);
  sched.addTask(pControllerTask);
  sched.addTask(pValveTask);
#endif

#ifdef __TESTING_HW__
  /* Testing */
  Task* pTestHWTask = new TestHWTask(pHWPlatform);
  pTestHWTask->init(2000);
  sched.addTask(pTestHWTask);
#endif
}

void loop() {
  sched.schedule();
}