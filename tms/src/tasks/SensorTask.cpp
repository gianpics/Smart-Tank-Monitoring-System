#include "SensorTask.h"
#include "kernel/Logger.h"

SensorTask::SensorTask(Sonar* pSonar, QueueHandle_t sensorQueue) : pSonar(pSonar), xSensorQueue(sensorQueue)
{
    state = QUERY;
}

void SensorTask::tick(){
    switch (state) {
        case QUERY: {
            if (this->checkAndSetJustEntered()){
                Logger.log(F("[ST] QUERY"));
                lastQueryTimestamp = 0;
            }

            if(millis() - lastQueryTimestamp > QUERY_PERIOD){
                lastQueryTimestamp = millis();
                float value = pSonar->getDistance();
                Logger.log("Distance: " + String(value));
                if(value > NO_OBJ_DETECTED) {
                    xQueueOverwrite(xSensorQueue, &value);
                }
            }
            break;
        }
    }
}

bool SensorTask::checkAndSetJustEntered(){
    bool bak = justEntered;
    if (justEntered){
      justEntered = false;
    }
    return bak;
}
