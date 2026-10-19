#include "Pot.h"
#include "Arduino.h"

Potentiometer::Potentiometer(int pin){
  this->pin = pin;
  sync();
} 
  
void Potentiometer::sync(){
  value = analogRead(pin);
  updateSyncTime(millis());
}

float Potentiometer::getValue(){
  // inverted so that max rotation = 100% (open), matching the DBS slider's direction
  return 1.0 - (value / 1023.0);
}

void Potentiometer::updateSyncTime(long time){
	lastTimeSync = time;
}

long Potentiometer::getLastSyncTime(){
	return lastTimeSync;
}
