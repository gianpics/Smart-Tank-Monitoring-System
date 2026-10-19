#include "CommTask.h"
#include "kernel/Logger.h"

CommTask::CommTask(Led *pLedG, Led *pLedR, QueueHandle_t sensorQueue) : pLedG(pLedG), pLedR(pLedR), xSensorQueue(sensorQueue) {
    pClient = new PubSubClient(espClient);
    setState(TaskState::INIT);
    lastMqttAttempt = -MQTT_CONN_ATTEMPT_COOLDOWN;
}

void CommTask::tick(){
    switch (state) {
        case TaskState::INIT:{
            if (this->checkAndSetJustEntered()) {
                Logger.log(F("[CT] INIT"));
                setLEDs();
                if(WiFi.status() != WL_CONNECTED) {
                    setupWifi();
                }
            }

            if(WiFi.status() != WL_CONNECTED) {
                Logger.log(".");
                break;
            }

            if(!pClient->connected()) {
                if (this->elapsedTimeInState() - lastMqttAttempt > MQTT_CONN_ATTEMPT_COOLDOWN) {
                    lastMqttAttempt = elapsedTimeInState();
                    connectMqttClient();
                }
                break;
            }
            
            if(WiFi.status() == WL_CONNECTED && pClient->connected()){ 
                Logger.log("WiFi connected");
                Logger.log("IP address: ");
                Logger.log(WiFi.localIP().toString());
            
                setState(TaskState::CONNECTED);
                return;
            }

            break;
        }
        case TaskState::CONNECTED: {
            if (this->checkAndSetJustEntered()){
                Logger.log(F("[CT] CONNECTED"));
                setLEDs();
            }

            pClient->loop();

            if(WiFi.status() != WL_CONNECTED){
                Logger.log(F("[CT] WiFi lost"));
                setState(TaskState::INIT);
                return;
            }

            if(!pClient->connected()){
                Logger.log("[CT] MQTT lost, rc=" + String(pClient->state()));
                setState(TaskState::INIT);
                return;
            }
            send();
            break;
        }
    }
}

void CommTask::setupWifi() {
    Logger.log(String("Connecting to ") + SSID_WIFI);
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID_WIFI, PASSWORD_WIFI);
}

void CommTask::connectMqttClient() {

    randomSeed(micros()); // seed for random client Id generation at reconnection

    pClient->setServer(MQTT_SERVER, MQTT_PORT);

    // Create a random client ID
    String clientId = String("esiot-2025-client-")+String(random(0xffff), HEX);

    // Attempt to connect
    if (pClient->connect(clientId.c_str())) {
        Logger.log("MQTT client connected");
    } else {
        Logger.log("MQTT client failed, rc=" + String(pClient->state()));
    }
  
}

void CommTask::send(){
    float value;
    if( xQueueReceive(xSensorQueue, &value, 10) ){
        snprintf (msg, MSG_BUFFER_SIZE, String(value).c_str());
        pClient->publish(TOPIC, msg); 
    }

}

void CommTask::setState(int s){
    state = (TaskState)s;
    stateTimestamp = millis();
    justEntered = true;
}

long CommTask::elapsedTimeInState(){
    return millis() - stateTimestamp;
}

bool CommTask::checkAndSetJustEntered(){
    bool bak = justEntered;
    if (justEntered){
      justEntered = false;
    }
    return bak;
}

void CommTask::setLEDs()
{
    if(state == TaskState::CONNECTED){
        pLedG->switchOn();
        pLedR->switchOff();
    } else {
        pLedG->switchOff();
        pLedR->switchOn();
    }
}
