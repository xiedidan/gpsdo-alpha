#include <SPI.h>
#include "common.h"
#include "filter.h"
#include "freqReader.h"
#include "freqWriter.h"
#include "pid.h"
#include "lcd1602.h"
#include "eventLoop.h"
#include "protocol.h"

void setup()
{
  Serial.begin(115200);
  
  initFreqReader();
  initGSIP();
  // TODO : init PID and go into automatic mode
}

void loop()
{
  getMsg();

  processEvent();
}

// #define LCD_TEST

#ifdef LCD_TEST
void setup()
{
  SPI.begin();
  initLcd();
  printLcd("Hello", 5);
}

void loop()
{
  delay(1000);
}
#endif
