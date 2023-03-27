
#include <stdlib.h>
/*
Generate a stack overflow 
*/
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu=0;
#else
static const BaseType_t app_cpu=1;
#endif

/* settings */  
static const int msg_queue_len=5;

//globals
//define golbal queue to be able to access at
static QueueHandle_t msg_queue;

/* Generate some taks*/  
void print_messegaes(void *parameter)
{
  //store value we read from queue
  int item; 
  while(1)
  {
    /*addres of local vaiable where we copy to
     0 is the timeout waiting (0 return immediatly)
      if there's a message in the queue return true if not 
      return false (do not block)
      NOTE: 
      each time we read from queue we dequeue it
      so if speed of writing> reading then it will overflow
      if speed receiving > writing could read if
       outside condition same variable many time
    */
    if(xQueueReceive(msg_queue,(void *)&item,0)== pdTRUE){ 
      Serial.println(item);
    }
    //outside cond
    //Serial.println(item);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

//*************************************************
void setup() 
{
  //very slow baudrate to be able to watch
  Serial.begin(9600);
  // Wait a moment to start (so we don't miss Serial output)
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println(" ___FREERTOS Queue DEMO _____");
  
  //create queue
  msg_queue = xQueueCreate(msg_queue_len, sizeof(int));//size 5
  
  xTaskCreatePinnedToCore(
    print_messegaes,
    "print_messegaes",
    1024, //stack size words of rtos ,smallest stack size if 768
    NULL, //PARAMETer to pass to function
    1,  //set max priority to the task(0,)
    NULL, //task handle
    app_cpu //run on one core (esp32 only)
    );
  
}

void loop()
{ 
    static int num =0;
    /*/ timeout after 10 tick for space to become available
    we send here to the queue with timeout of 10
    */
    if(xQueueSend(msg_queue,(void *)&num,10) != pdTRUE)
    {
      Serial.println("Queue is full");
    }
     num++;
    // Wait before trying again
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}


