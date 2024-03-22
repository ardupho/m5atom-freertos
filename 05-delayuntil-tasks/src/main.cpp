#include <Arduino.h>
#include "M5Atom.h"

/*
La función xTaskDelayUntil() podría ser todo lo que necesita. Esta llamada 
a la API permite a la persona que llama aplazar la ejecución (bloquear una tarea) 
hasta que llegue un tiempo específico futuro. Para mantener la precisión del tiempo, 
el argumento uno es tanto un tiempo de entrada como un valor actualizado de ticks de salida. 
El argumento dos se usa para programar la próxima hora de activación, 
tantos tics del valor del argumento uno.
*/
/*
Cuando comience la demostración, los dos LED parecerán parpadear al unísono. 
A medida que pasa el tiempo, el LED1 (tarea 1) comenzará a atrasarse con 
respecto al cronograma que mantiene el LED2 (tarea 2). Esto ocurre porque 
la llamada a delay() es relativa a la hora en que se llamó. 
El cálculo realizado en big_think() hará que la llamada a delay() sea un poco 
más tarde con cada ciclo que pasa.
*/

static volatile bool startf = false;
static TickType_t period = 250;

static void big_think() {
  for ( int x=0; x<40000; ++x )
    __asm__ __volatile__ ("nop");
}

static void task1(void *argp) {
  bool state = true;
  while ( !startf );

  for (;;) {
    state ^= true;
    M5.dis.drawpix(1, state?0xff00ff:0x000000);        
    big_think();
    delay(period);
  }
}

static void task2(void *argp) {
  bool state = true;
  while ( !startf );
  TickType_t ticktime = xTaskGetTickCount();

  for (;;) {
    state ^= true;
    M5.dis.drawpix(0, state?0xff0000:0x000000);            
    big_think();
    vTaskDelayUntil(&ticktime,period);
  }
}

//
// Initialization:
//
void setup() {
  
  M5.begin(true, false, true);    // Init Atom-Matrix(Initialize serial port, I2C, LED). 
  delay(50);                      // delay 50ms.
  
  int app_cpu = xPortGetCoreID();
  BaseType_t rc;

  // delay(2000); // Allow USB to connect
  M5.dis.drawpix(0, 0xff0000);
  M5.dis.drawpix(1, 0xff00ff);                        

  rc = xTaskCreatePinnedToCore(
    task1,
    "task1",
    2048,
    nullptr,
    1,
    nullptr,
    app_cpu);
  assert(rc == pdPASS);

  rc = xTaskCreatePinnedToCore(
    task2,
    "task2",
    2048,
    nullptr,
    1,
    nullptr,
    app_cpu);
  assert(rc == pdPASS);

  startf = true;
}

void loop() {
  delay(50);
}