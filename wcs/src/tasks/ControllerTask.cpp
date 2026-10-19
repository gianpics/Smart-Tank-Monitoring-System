#include "ControllerTask.h"
#include <Arduino.h>
#include "config.h"
#include <math.h>
#include "kernel/Logger.h"
 
ControllerTask::ControllerTask(Button* pButton, Lcd* pLcd, Potentiometer* pPot, Context* pContext): 
    pButton(pButton), pLcd(pLcd), pPot(pPot), pContext(pContext){
    resetTask();
}
  
void ControllerTask::tick(){
    switch (state){    
        case Context::ControllerStates::AUTO: {
            if (this->checkAndSetJustEntered()){
                Logger.log(F("[CT] AUTO"));
                pContext->resetRequest();
                updateLCD();
            }

            if(pContext->isUnconnected()){
                setState(Context::ControllerStates::UNCONNECTED);
                return;
            }

            if(isManualRequested()){
                setState(Context::ControllerStates::MANUAL);
                return;
            }
            
            if(pContext->validRemoteRequest){
                updateValveWithRemotePosition();
                pContext->resetRequest();
                updateLCD();
            }

            break;
        }
        case Context::ControllerStates::MANUAL: {
            if (this->checkAndSetJustEntered()){
                Logger.log(F("[CT] MANUAL_LOCAL"));
                resetPreviousControlsValues();
                pContext->resetRequest();
                updateLCD();
            }

            if(pContext->isUnconnected()){
                setState(Context::ControllerStates::UNCONNECTED);
                return;
            }

            if(isAutoRequested()){
                setState(Context::ControllerStates::AUTO);
                return;
            }

            // remote position
            if(pContext->isPositionRemotelyRequested()){
                updateValveWithRemotePosition();
                pContext->resetRequest();
                updateLCD();
            }

            // local position
            int potValue = getPotentiometerValue();
            // ignoring noise (value diff < 2 degs)
            if(abs(potValue - prevPotValue) > 2 && potValue != pContext->currentOpeningDeg){
                prevPotValue = potValue;
                pContext->requestedOpeningDeg = potValue;
                updateLCD();
            }

            break;
        }
        case Context::ControllerStates::UNCONNECTED: {
            if (this->checkAndSetJustEntered()){
                Logger.log(F("[CT] UNCONNECTED"));
                pContext->resetRequest();
                updateLCD();
            }

            if(pContext->isConnected()){
                setState(previousState);
                return;
            }
            break;
        }
    }
}

void ControllerTask::resetTask()
{
    setState(Context::ControllerStates::AUTO);
}

void ControllerTask::setState(int s){
    previousState = state;
    state = (Context::ControllerStates)s;
    pContext->setControllerState((Context::ControllerStates)s);
    stateTimestamp = millis();
    justEntered = true;
}

long ControllerTask::elapsedTimeInState(){
    return millis() - stateTimestamp;
}

bool ControllerTask::checkAndSetJustEntered(){
    bool bak = justEntered;
    if (justEntered){
      justEntered = false;
    }
    return bak;
}

void ControllerTask::updateLCD()
{
    pLcd->clearLCD();
    pLcd->display(pContext->getControllerStateToString() + "   ", LCD_ROW_POS_1, LCD_COL_POS_1);
    // use requestedOpeningDeg to reflect the actual position
    int perc = round((pContext->requestedOpeningDeg * 100.0) / VALVE_OPENING_DEG);
    pLcd->display(String(perc) + "%    ", LCD_ROW_POS_2, LCD_COL_POS_2);
}

bool ControllerTask::isManualRequested()
{
    return (pContext->isManualRemotelyRequested() || pButton->isPressed()) && pressDebounce();
}

bool ControllerTask::isAutoRequested()
{
    return (pContext->isAutoRemotelyRequested() || pButton->isPressed()) && pressDebounce();
}

bool ControllerTask::pressDebounce()
{
    if (this->elapsedTimeInState() > BTN_DEBOUNCE_TIME) return true;
    return false;
}

void ControllerTask::resetPreviousControlsValues()
{
    prevRemoteVal = pContext->isManualRemotelyRequested() ? pContext->remotelyRequestedOpening : 50;
    prevPotValue = getPotentiometerValue();
}

void ControllerTask::updateValveWithRemotePosition()
{
    int remoteVal = pContext->remotelyRequestedOpening;
    if(remoteVal != prevRemoteVal && remoteVal != pContext->currentOpeningDeg){
        prevRemoteVal = remoteVal;
        pContext->requestedOpeningDeg = remoteVal;
    }
}

int ControllerTask::getPotentiometerValue()
{
    if(millis() - pPot->getLastSyncTime() > POT_SYNC_EXPIRATION){
        pPot->sync();
    }
    return round(pPot->getValue() * VALVE_OPENING_DEG);
}
