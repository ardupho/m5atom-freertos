#include <Arduino.h>
#include "M5Atom.h"

struct s_led {
  byte          lednum;	  // LED number
  byte          state;	  // LED state
  unsigned      napms;    // Delay to use (ms)
  TaskHandle_t  taskh;    // Task handle
};

static s_led leds[3] = {
  { 0, 0, 500, 0 },
  { 2, 0, 200, 0 },
  { 4, 0, 750, 0 }
};

static void led_task_func(void *argp) {
  s_led *ledp = (s_led*)argp;
  unsigned stack_hwm = 0, temp;

  vTaskDelay( 1000 / portTICK_PERIOD_MS );
  
  for (;;) {
    ledp->state ^= 1;
    if (ledp->state == 0){
      M5.dis.drawpix(ledp->lednum, 0x000000);
    } else {
      if(ledp->lednum==0) M5.dis.drawpix(ledp->lednum, 0xff0000);
      if(ledp->lednum==2) M5.dis.drawpix(ledp->lednum, 0x00ff00);
      if(ledp->lednum==4) M5.dis.drawpix(ledp->lednum, 0x0000ff);
    }
    //digitalWrite(ledp->gpio,ledp->state ^= 1);
    temp = uxTaskGetStackHighWaterMark(nullptr);
    if ( !stack_hwm || temp < stack_hwm ) {
      stack_hwm = temp;
      printf("Task for LedNum %d has stack hwm %u\n",
        ledp->lednum,stack_hwm);
    }
    delay(ledp->napms);
  }
}


void setup() {
  // put your setup code here, to run once:
  //Borre el búfer del puerto serie, establezca la velocidad en baudios del puerto serie en 115200; inicialice I2C; inicialice la matriz de LED
  M5.begin(true, false, true);    // Init Atom-Matrix(Initialize serial port, I2C, LED). 
  delay(50);                      // delay 50ms.
  M5.dis.fillpix(0x000000);
  int app_cpu = 0;    // CPU number
  delay(500);       // Pause for serial setup
  app_cpu = xPortGetCoreID();
  printf("app_cpu is %d (%s core)\n",
    app_cpu,
    app_cpu > 0 ? "Dual" : "Single");

  printf("LEDs on gpios: ");
  for ( auto& led : leds ) {
    M5.dis.drawpix(led.lednum, 0x000000);
    xTaskCreatePinnedToCore(
      led_task_func,
      "led_task",
      2048,
      &led,
      1,
      &led.taskh,
      app_cpu
    );
    printf("%d ",led.lednum);
  }
  putchar('\n');
}


void loop() {
  vTaskDelay( 1000 / portTICK_PERIOD_MS );
}
