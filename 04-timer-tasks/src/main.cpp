#include <Arduino.h>
#include "M5Atom.h"

/*
La demostración de este programa usa una clase llamada AlertLED, 
que activa un LED de una manera llamativa para indicar algún error o 
falla crítica. La instancia de clase se crea con el número de LED 
que se activará y el período de parpadeo (1000 ms por defecto). 
Cuando se activa para mostrar una alerta, el LED parpadea rápidamente 5 veces, 
luego se detiene por el resto del período antes de repetir. 
*/

//
// AlertLED class to drive LED
//
class AlertLED {
  TimerHandle_t     thandle = nullptr;
  volatile bool     state;
  volatile unsigned count;
  unsigned          period_ms;
  int               nled;

  void reset(bool s);

public:
  AlertLED(int nled,unsigned period_ms=1000);
  void alert();
  void cancel();
  static void callback(TimerHandle_t th);
};

//
// Constructor:
//  nled        led de la matriz a encender
//  period_ms   Overall period in ms
//
AlertLED::AlertLED(int nled,unsigned period_ms) {
  this->nled = nled;
  this->period_ms = period_ms;
}

//
// Internal method to reset values
//
void AlertLED::reset(bool s) {
  state = s;
  count = 0;
  M5.dis.drawpix(this->nled, s?0xff0000:0x000000);    
}  

//
// Method to start the alert:
//
void AlertLED::alert() {

  if ( !thandle ) {
    thandle = xTimerCreate(
      "alert_tmr",
      pdMS_TO_TICKS(period_ms/20),
      pdTRUE,
      this,
      AlertLED::callback);
    assert(thandle);
  }
  reset(true);
  xTimerStart(thandle,portMAX_DELAY);
}

//
// Method to stop an alert:
//
void AlertLED::cancel() {
  if ( thandle ) {
    xTimerStop(thandle,portMAX_DELAY);
    M5.dis.drawpix(this->nled,0x000000);        
  }
}

// static method, acting as the
// timer callback:
//
void AlertLED::callback(TimerHandle_t th) {
  AlertLED *obj = (AlertLED*)pvTimerGetTimerID(th);

  assert(obj->thandle == th);
  obj->state ^= true;
  M5.dis.drawpix(obj->nled, obj->state?0xff0000:0x000000);      

  if ( ++obj->count >= 5 * 2 ) {
    obj->reset(true);
    xTimerChangePeriod(th,pdMS_TO_TICKS(obj->period_ms/20),portMAX_DELAY);
  } else if ( obj->count == 5 * 2 - 1 ) {
    xTimerChangePeriod(th,
      pdMS_TO_TICKS(obj->period_ms/20+obj->period_ms/2),
      portMAX_DELAY);
    assert(!obj->state);
  }
}


//
// Global objects
//
static AlertLED alert1(0,1000);
static AlertLED alert2(24,1000);
static unsigned loop_count = 0;

//
// Initialization:
//

void setup() {
// delay(2000); // Allow USB to connect
  M5.begin(true, false, true);    // Init Atom-Matrix(Initialize serial port, I2C, LED).
  delay(50);                      // delay 50ms.
  alert1.alert();
  alert2.alert();
}

void loop() {
   if ( loop_count >= 70 ) {
    alert1.alert();
    alert2.alert();
    loop_count = 0;
  }

  delay(100);

  if ( ++loop_count >= 50 ){
    alert1.cancel();
    alert2.cancel();
  }

}