/*
 * FreeRTOS Counting Semaphore Challenge
 * 
 * Challenge: use a mutex and counting semaphores to protect the shared buffer 
 * so that each number (0 throguh 4) is printed exactly 3 times to the Serial 
 * monitor (in any order). Do not use queues to do this!
 * 
 * Hint: you will need 2 counting semaphores in addition to the mutex, one for 
 * remembering number of filled slots in the buffer and another for 
 * remembering the number of empty slots in the buffer.
 */

// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

//seetings
enum {buf_size=5}; //size of buffer
static const int num_produc_tasks =5;
static const int num_cons_tasks =2;
static const int num_writers =3;// num of each producer writes to buf

//globals
static int buf[buf_size]; //shared buff (arr)
//we need to counter for num of empty element and full element
static int head=0; // Writing index to buffer
static int tail=0; // Reading index to buffer

//semaphore
static SemaphoreHandle_t bin_sem;     // Waits for parameter to be read

//tasks
// Producer: write a given number of times to shared buffer
void producer(void *parameters)
{
  int num=*(int *)parameters;//typecasting

  // Release the binary semaphore 
  xSemaphoreGive(bin_sem);

  //fill shared buffer with task num
  for(int i=0; i<num_writers ; i++)
  {
    //critical section
    buf[head]=num;
    head=(head+1) % buf_size;
  }
  // Delete self task
  vTaskDelete(NULL);
}
// Consumer: continuously read from shared buffer
void consumer(void *parameters)
{
  int val;
  while(1){
    // Critical section (accessing shared buffer and Serial)
    val = buf[tail];
    tail = (tail + 1) % buf_size;
    Serial.println(val);
  }
}

void setup(){

  char task_name[12];
  // Configure Serial
  Serial.begin(115200);

  // Wait a moment to start (so we don't miss Serial output)
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS Semaphore Alternate Solution---");


  // Create mutexes and semaphores before starting tasks
  bin_sem = xSemaphoreCreateBinary();

  //start producer tasks (wait for each to read argument)
  for (int i = 0; i < num_produc_tasks; i++)
  {
    sprintf(task_name ,"producer %i",i);
    xTaskCreatePinnedToCore(producer,
                            task_name,
                            1024,
                            (void *)&i,
                            1,
                            NULL,
                            app_cpu);
    xSemaphoreTake(bin_sem, portMAX_DELAY);                        
  }

   // Start consumer tasks
  for (int i = 0; i < num_cons_tasks; i++) {
    sprintf(task_name, "Consumer %i", i);
    xTaskCreatePinnedToCore(consumer,
                            task_name,
                            1024,
                            NULL,
                            1,
                            NULL,
                            app_cpu);
  }

  // Notify that all tasks have been created
  Serial.println("All tasks created");
}


void loop() {

  // Do nothing but allow yielding to lower-priority tasks
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}