#ifndef __SENSOR_TASK__
#define __SENSOR_TASK__

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "kernel/Task.h"
#include "config.h"
#include "devices/Sonar.h"


class SensorTask: public Task {

public:
    SensorTask(Sonar* pSonar, QueueHandle_t sensorQueue);
    void tick();
    bool checkAndSetJustEntered();

private:

    long lastQueryTimestamp;

    enum { QUERY } state;
    long safeDistanceTimestamp;
    long stateTimestamp;
    bool justEntered;

    Sonar* pSonar;
    QueueHandle_t xSensorQueue;
};

#endif