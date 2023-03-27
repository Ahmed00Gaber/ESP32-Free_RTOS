/*
Generate a stack overflow 
*/
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu=0;
#else
static const BaseType_t app_cpu=1;
#endif

// Globals
static int shared_var = 0;

//semaphore similar to mutex
static SemaphoreHandle_t mutex;
/* Generate some taks*/  
void incTask(void *parameter)
{
  //store value we read from queue
  int local_var;
  
  while(1)
  {
    // take mutex prior to critcal section (key) 0 not avilable
    //0 ticks if it can't take mutex return flase value
    if(xSemaphoreTake(mutex,0)== pdTRUE)
    {
    local_var= shared_var;
    local_var++;
    //random dealy because when 2nd delay > 1st it will interrupt it cause race cond
    vTaskDelay(100 / portTICK_PERIOD_MS);
    shared_var= local_var;
    //give mutex back after finish
    xSemaphoreGive(mutex);
    Serial.println(shared_var);
    }
    else
    {
      //if we wan't to create something here
    }
    
  }
}

//*************************************************
void setup() 
{
  //very slow baudrate to be able to watch
  Serial.begin(9600);
  //delay 1 sec before starting
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println(" ___FREERTOS Mutex DEMO _____");
  
  //creating mutex before starting tasks
  mutex=xSemaphoreCreateMutex();
  xTaskCreatePinnedToCore(
    incTask,
    "increment Task1",
    1024, //stack size words of rtos ,smallest stack size if 768
    NULL, //PARAMETer to pass to function
    1,  //priority
    NULL, //task handle
    app_cpu //run on one core (esp32 only)
    );

  xTaskCreatePinnedToCore(
    incTask,
    "increment Task2",
    1024, //stack size words of rtos ,smallest stack size if 768
    NULL, //PARAMETer to pass to function
    1,  //priority
    NULL, //task handle
    app_cpu //run on one core (esp32 only)
    );
  
    // Delete "setup and loop" task
    vTaskDelete(NULL);//to make our task is the only used
}

void loop()
{ 

}


