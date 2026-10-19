#ifndef __CONTEXT__
#define __CONTEXT__

#include <Arduino.h>

class Context {

public:
  Context();

  enum ControllerStates { UNCONNECTED, AUTO, MANUAL };

  // Controller
  ControllerStates getControllerState();
  void setControllerState(int state);
  String getControllerStateToString();


  // Interface
  ControllerStates remotelyRequestedState;
  int remotelyRequestedOpening;
  bool validRemoteRequest;
  bool isUnconnected();
  bool isConnected();
  bool isPositionRemotelyRequested();
  bool isManualRemotelyRequested();
  bool isAutoRemotelyRequested();
  void resetRequest();

  // Valve
  int requestedOpeningDeg;
  int currentOpeningDeg;
  int getValveOpeningPercentageValue();
  String getValveOpeningPercentage();

private:
  ControllerStates controllerState;


};

#endif
