// use only 1 core
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu=0;
#else
static const BaseType_t app_cpu=1;
#endif

//some string to print
const char msg[] ="ahmed is running";

//task handle
static TaskHandle_t task_1 = NULL;
static TaskHandle_t task_2 = NULL;

//tasks

//print with lower priority
void startTask1 (void *parameter)
{
  int msg_len =strlen(msg);
  while(1)
  {
    Serial.println();//means we join task1 again
    for(int i=0; i<msg_len ;i++){
      Serial.print(msg[i]);
    }
    Serial.println();
    vTaskDelay(1000 / portTICK_PERIOD_MS); //1ms   
  }
}

//it will print this task till delay of task1 finish then start it again
void startTask2 (void *parameter)
{
  while(1)
  {
    Serial.print('*');
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
void setup() {
  //very slow baudrate to be able to watch
  Serial.begin(300);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  
  Serial.println();
  Serial.println(" ___ freeRTOS  DEMO _____");
  
//print our core and priority
  Serial.print(" setup and loop task running on core ");
  Serial.print(xPortGetCoreID());
  Serial.print(" with priority ");
  Serial.print(uxTaskPriorityGet(NULL));



  //task run forever
  xTaskCreatePinnedToCore(
    startTask1,
    "Task 1",
    1024, //stack size words of rtos ,smallest stack size if 768
    NULL, //PARAMETer to pass to function
    1,  //lower priority
    &task_1, //task handle
    app_cpu //run on one core (esp32 only)
    );//


  //task to run once with higher priority
  xTaskCreatePinnedToCore(
    startTask2,
    "TASK 2",
    1024, //stack size words of rtos ,smallest stack size if 768
    NULL, //PARAMETer to pass to function
    2,  //higher priority
    &task_2, //task handle
    app_cpu //run on one core (esp32 only)
    );//

}

void loop() {
  //suspend the higher priority task for some more interval
  //each 2 swc you will find an astrict
  for(int i=0; i<3 ;i++){
      vTaskSuspend(task_2);
      vTaskDelay(2000 / portTICK_PERIOD_MS);
      vTaskResume(task_2);
      vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
  // delete lower priority task
  // if(task_1 != NULL)
  // {
  //   //tale care when deleting a non exsisting task you probably may delete exsisting memory
  //   vTaskDelete(task_1);
  //   task_1=NULL;
  // }
}
