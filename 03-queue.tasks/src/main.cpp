#include <Arduino.h>
#include "M5Atom.h"

// https://github.com/ve3wwg/FreeRTOS_for_ESP32

static QueueHandle_t queue;

//
// Button task:
//
static void button_task(void *argp) {

  bool event;
  while (1)
  {
    M5.update(); // need to call update()
    if (M5.Btn.wasPressed()) 
    {  // Check if the key is pressed.
      event = true;
      //event ^= true;
      if ( xQueueSendToBack(queue,&event,1) == pdPASS )
        Serial.println("Button is pressed");         
    }
    taskYIELD();         
  }  
}

//
// LED queue receiving task
//
static void led_task(void *argp) {
  
  BaseType_t s;
  bool event, led = false;
  
  while (1)  
  {
    s = xQueueReceive(queue,&event,portMAX_DELAY);
    assert(s == pdPASS);

    if ( event ) {
      // Button press:
      // Toggle LED
      led ^= true;
      if (led == true)
      {
        M5.dis.fillpix(0xFFFF00);
        M5.dis.setBrightness(100);        
      } else {
        M5.dis.fillpix(0xffffff); //Illuminate the entire LED lattice with RBG color 0xffffff. 
        M5.dis.setBrightness(10); //Set the brightness of the illuminated LED
      }     
    }
 
  }  
}

void setup() {
  // put your setup code here, to run once:
  int app_cpu = xPortGetCoreID();
  TaskHandle_t h;
  BaseType_t rc;

  delay(2000);          // Allow USB to connect
  queue = xQueueCreate(40,sizeof(bool));
  assert(queue);
  
  M5.begin(true, false, true);    // Init Atom-Matrix(Initialize serial port, I2C, LED). 
  delay(50);                      // delay 50ms.
  
  rc = xTaskCreatePinnedToCore(
    button_task,
    "button",
    2048,     // Stack size
    nullptr,  // No args
    1,        // Priority
    &h,       // Task handle
    app_cpu   // CPU
  );
  assert(rc == pdPASS);
  assert(h);

  rc = xTaskCreatePinnedToCore(
    led_task,
    "led",
    2048,     // Stack size
    nullptr,  // Not used
    1,        // Priority
    &h,       // Task handle
    app_cpu   // CPU
  );
  assert(rc == pdPASS);
  assert(h);

}

void loop() {  
 
  vTaskDelete(nullptr);
}