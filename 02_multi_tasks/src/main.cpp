#include <Arduino.h>
#include "M5Atom.h"

void TaskBlink(void *pvParameters)
{
}

void setup() {
  // put your setup code here, to run once:
  //Borre el búfer del puerto serie, establezca la velocidad en baudios del puerto serie en 115200; inicialice I2C; inicialice la matriz de LED
  M5.begin(true, false, true);    // Init Atom-Matrix(Initialize serial port, I2C, LED). 
  delay(50);                      // delay 50ms.
  //M5.dis.drawpix(0, 0x00ff00);    // Light the LED with the specified RGB color
  
}

void loop() {
  vTaskDelay( 1000 / portTICK_PERIOD_MS );
}
