#include "ValveTask.h"
#include <Arduino.h>
#include "config.h"
#include "kernel/Logger.h"

ValveTask::ValveTask(ServoMotor* pValve, Context* pContext): 
    pValve(pValve), pContext(pContext){
    setState(TaskStates::IDLE);
    pContext->currentOpeningDeg = 0;
}
  
void ValveTask::tick(){
    switch (state){ 
        case TaskStates::IDLE: {
            if (this->checkAndSetJustEntered()){
                Logger.log(F("[VT] IDLE"));
                pValve->off();
            }

            if (pContext->requestedOpeningDeg != pContext->currentOpeningDeg) {
                setState(TaskStates::MOVING);
            }

            break; 
        }
        case TaskStates::MOVING: {
            if (this->checkAndSetJustEntered()){
                Logger.log(F("[VT] MOVING"));

                pContext->currentOpeningDeg = pContext->requestedOpeningDeg;
                pValve->on();
                pValve->setPosition(pContext->currentOpeningDeg);
            }

            if (elapsedTimeInState() > SERVO_ADJUST_TIME) {
                setState(TaskStates::IDLE);
            }

            break;
        }
    }
}

void ValveTask::setState(int s){
    state = s;
    stateTimestamp = millis();
    justEntered = true;
}

long ValveTask::elapsedTimeInState(){
    return millis() - stateTimestamp;
}

bool ValveTask::checkAndSetJustEntered(){
    bool bak = justEntered;
    if (justEntered){
      justEntered = false;
    }
    return bak;
}
