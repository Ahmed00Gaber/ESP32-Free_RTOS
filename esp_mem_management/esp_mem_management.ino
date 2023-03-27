/*
Generate a Memory location watch stack 
*/
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu=0;
#else
static const BaseType_t app_cpu=1;
#endif


/* Generate some taks*/  

void test_task(void *parameter)
{
  while(1)
  {
    int a=1;
    int b[100]; //arr size 100 for 4 byte we need 400 byte
    for(int i=0;i<100;i++)
    {
      b[i]=a+1; //print 2
    }
    Serial.println(b[0]);

    //print ore remamning stack memory (word) , we need to *4
    Serial.print("remaning memory(stack) words: ");
    Serial.println(uxTaskGetStackHighWaterMark(NULL));

    //number of free heap memory bytes before malloc bytes()
    Serial.print("heap before malloc(bytes) : ");
    Serial.println(xPortGetFreeHeapSize());

    /*allocate memory we may use normal malllo()(it's not thread safe) 
    or rtos prPortMalloc()
    it return NULL if it's out of heap memory
    */
    int *ptr=(int*)pvPortMalloc(1024 * sizeof(int));//vanila rtos 
    
    //heap out of memory
    if(ptr==NULL)
    {
      Serial.println("Not enoUgh heap");
    }
    else
    {
      //do something with heap memory for so itsn't removed by comiler
      for(int i=0;i<1024;i++)
      {
        ptr[i]=3; //print 2
      }
    }
    //number of free heap memory bytes After malloc bytes()
    Serial.print("heap After malloc(bytes) : ");
    Serial.println(xPortGetFreeHeapSize());

    //free up our allocated memory
    vPortFree(ptr);

    //delay
    vTaskDelay(500 / portTICK_PERIOD_MS);

  }
}

void setup() 
{
  //very slow baudrate to be able to watch
  Serial.begin(9600);
  Serial.println();
  Serial.println(" ___ Memory management DEMO _____");
  
    //TASk TO RUN FOREVER on which core

  /*
  Note: 
  1-using 1024 size will cause overflow so we need more space
  2-
  */
  xTaskCreatePinnedToCore(
    test_task,
    "Test memory",
    1500, //stack size words of rtos ,smallest stack size if 768
    NULL, //PARAMETer to pass to function
    1,  //set max priority to the task(0,)
    NULL, //task handle
    app_cpu //run on one core (esp32 only)
    );
  
    // Delete "setup and loop" task
    vTaskDelete(NULL);//to make our task is the only used
}

void loop()
{ 
    //never join this part
}


