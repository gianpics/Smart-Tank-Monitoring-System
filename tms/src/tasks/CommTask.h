#ifndef __COMM_TASK__
#define __COMM_TASK__

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "kernel/Task.h"
#include "config.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "devices/Led.h"


class CommTask: public Task {

public:
    CommTask(Led* pLedG, Led* pLedR, QueueHandle_t xSensorQueue);
    void tick();
    void setState(int state);
    long elapsedTimeInState();
    bool checkAndSetJustEntered();

    void setLEDs();
    void setupWifi();
    void connectMqttClient();
    void send();

private:

    enum TaskState { INIT, CONNECTED } state;
    long safeDistanceTimestamp;
    long stateTimestamp;
    bool justEntered;

    long lastMqttAttempt;
    char msg[MSG_BUFFER_SIZE];

    Led* pLedG;
    Led* pLedR;
    WiFiClient espClient;
    PubSubClient* pClient;
    QueueHandle_t xSensorQueue;
};

#endif