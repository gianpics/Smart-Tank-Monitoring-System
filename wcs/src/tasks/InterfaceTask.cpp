#include <Arduino.h>
#include "config.h"
#include "kernel/Logger.h"
#include "InterfaceTask.h"

InterfaceTask::InterfaceTask(Context* pContext): pContext(pContext){
    setState(TaskStates::COMM);
}
  
void InterfaceTask::tick(){
    if (this->checkAndSetJustEntered()){
        Logger.log(F("[CT] COMM"));
    }
    
    parseSerial();

    updateCUS();
}

void InterfaceTask::setState(int s){
    state = s;
    stateTimestamp = millis();
    justEntered = true;
}

long InterfaceTask::elapsedTimeInState(){
    return millis() - stateTimestamp;
}

bool InterfaceTask::checkAndSetJustEntered(){
    bool bak = justEntered;
    if (justEntered){
      justEntered = false;
    }
    return bak;
}

void InterfaceTask::parseSerial()
{
    // Read from CUS
    String data = readSerial();
    if (data.length() > 0) {
        pContext->validRemoteRequest = processData(data);
    }
}

String InterfaceTask::readSerial()
{
    if (MsgService.isMsgAvailable()) {
        Msg* msg = MsgService.receiveMsg();
        String data = msg->getContent();
        delete msg; 
        data.trim();
        return data;
    }
    return "";
}

bool InterfaceTask::processData(String data)
{
    // <state;opening>
    // <[UNCONNECTED|AUTO|MANUAL];999>

    if (data.length() < 1 || !data.startsWith("<") || !data.endsWith(">")) return false;

    String inner = data.substring(1, data.length() - 1);
    int sep = inner.indexOf(';');
    if (sep < 0) return false;

    Context::ControllerStates state;
    if (!parseState(inner.substring(0, sep), state)) return false;

    String numStr = inner.substring(sep + 1);
    int opening = (numStr.length() > 0) ? numStr.toInt() : -1;
    if (opening != -1 && opening > 100) return false;

    int degrees = round(opening * VALVE_OPENING_DEG / 100);

    pContext->remotelyRequestedState = state;
    pContext->remotelyRequestedOpening = degrees;

    return true;
}

void InterfaceTask::updateCUS()
{
    String msg = "<";
    msg += pContext->getControllerStateToString();
    msg += ";";
    msg += pContext->getValveOpeningPercentageValue();
    msg += ">";
    MsgService.sendMsg(msg);
}

bool InterfaceTask::parseState(const String& s, Context::ControllerStates& out) {
    if (s == "UNCONNECTED") { out = Context::ControllerStates::UNCONNECTED; return true; }
    else if (s == "AUTO")   { out = Context::ControllerStates::AUTO;        return true; }
    else if (s == "MANUAL") { out = Context::ControllerStates::MANUAL;      return true; }
    
    return false;
}

