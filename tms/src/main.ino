#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "config.h"
#include "kernel/Logger.h"
#include "kernel/Task.h"
#include "tasks/CommTask.h"
#include "tasks/SensorTask.h"
#include "devices/Led.h"
#include "devices/Sonar.h"
#include "config.h"

//#define __TESTING_HW__

TaskHandle_t CommTaskHandle;
TaskHandle_t SensorTaskHandle;
TaskHandle_t TestTaskHandle;

QueueHandle_t xSensorQueue;

Led* pLedG = new Led(LED_G_PIN);
Led* pLedR = new Led(LED_2_PIN);
Sonar* pSonar = new Sonar(ECHO_PIN, TRIG_PIN, SONAR_MAX_TIME);


void Task1code(void* parameter){
  
  Serial.print("Task1 is running on core ");
  Serial.println(xPortGetCoreID());

  Task* pTask = (Task*)parameter;
  for (;;) {
    pTask->tick();
    delay(pTask->getPeriod());
  }
}

void Task2code(void* parameter){
  Serial.print("Task2 is running on core ");
  Serial.println(xPortGetCoreID());

  Task* pTask = (Task*)parameter;
  for (;;) {
    pTask->tick();
    delay(pTask->getPeriod());
  }
}

void TaskTestcode(void* parameter){
  Serial.print("TaskTest is running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {
    
    float distance = pSonar->getDistance();
    Logger.log("LEDs ON");
    pLedG->switchOn();
    pLedR->switchOn();
    Logger.log("Distance: " + String(distance));
    delay(3000);
    Logger.log("LEDs OFF");
    pLedG->switchOff();
    pLedR->switchOff();
    delay(1000);
    Logger.log("==========================");
  }
}


void setup() {
  Serial.begin(115200); 

  Logger.log(".::::: Tank Monitoring System :::::.");


#ifndef __TESTING_HW__
  xSensorQueue = xQueueCreate(1, sizeof(float));
  if(xSensorQueue == 0){
    Logger.log("Queue creation failed. Aborting...");
    return;
  }

  Task* pCommTask = new CommTask(pLedG, pLedR, xSensorQueue);
  pCommTask->init(1000);

  Task* pSensorTask = new SensorTask(pSonar, xSensorQueue);
  pSensorTask->init(1000);

  xTaskCreatePinnedToCore(Task1code,"CommTask",10000,(void*)pCommTask,1,&CommTaskHandle,0);                         
  delay(500); 

  xTaskCreatePinnedToCore(Task2code,"SensorTask",10000,(void*)pSensorTask,1,&SensorTaskHandle,1);          
  delay(500);

#endif

#ifdef __TESTING_HW__
  /* Testing */
  xTaskCreatePinnedToCore(TaskTestcode,"TestTask",10000,NULL,1,&TestTaskHandle,0);                         
  delay(500);
#endif

}


void loop() {
  /*/
  Serial.print("Main loop running on core ");
  Serial.println(xPortGetCoreID());
  delay(10000);
  */
}
