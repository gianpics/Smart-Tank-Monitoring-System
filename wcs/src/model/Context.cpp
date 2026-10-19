#include "Context.h"
#include "config.h"

Context::Context(){
  this->currentOpeningDeg = 0;
  this->requestedOpeningDeg = 0;
  this->remotelyRequestedOpening = -1;
}

Context::ControllerStates Context::getControllerState()
{
  return controllerState;
}

void Context::setControllerState(int state)
{
  controllerState = (ControllerStates)state;
}

String Context::getControllerStateToString()
{
  switch (controllerState) {
      default:
      case Context::ControllerStates::UNCONNECTED:    return UNCONNECTED_TEXT;
      case Context::ControllerStates::AUTO:           return AUTO_TEXT;
      case Context::ControllerStates::MANUAL:         return MANUAL_TEXT;
  }
}

bool Context::isUnconnected()
{
  return validRemoteRequest && remotelyRequestedState == Context::ControllerStates::UNCONNECTED;
}

bool Context::isConnected()
{
  return validRemoteRequest && remotelyRequestedState != Context::ControllerStates::UNCONNECTED;
}

bool Context::isPositionRemotelyRequested()
{
  return isManualRemotelyRequested() && remotelyRequestedOpening != -1;
}

bool Context::isManualRemotelyRequested()
{
  return validRemoteRequest && remotelyRequestedState == Context::ControllerStates::MANUAL;
}

bool Context::isAutoRemotelyRequested()
{
  return validRemoteRequest && remotelyRequestedState == Context::ControllerStates::AUTO;
}

void Context::resetRequest()
{
  //remotelyRequestedState = null;
  remotelyRequestedOpening = -1;
  validRemoteRequest = false;
}


int Context::getValveOpeningPercentageValue()
{
  float perc = ((currentOpeningDeg * 100.0) / VALVE_OPENING_DEG);
  return (int)round(perc);
}

String Context::getValveOpeningPercentage()
{
  return String(getValveOpeningPercentageValue()) + "%";
}