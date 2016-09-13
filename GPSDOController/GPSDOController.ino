#include <SPI.h>
#include "common.h"
#include "filter.h"
#include "freqReader.h"
#include "freqWriter.h"
#include "pid.h"
#include "lcd1602.h"

// must be power of 10
int counterInterval = 10;

void setup()
{
  Serial.begin(115200);
  initFreqReader();
  Serial.println("FreqCounterController");
}

int i = 0;
unsigned long sum = 0;

void loop()
{
  if (i < counterInterval)
  {
    sum += freq;
    i++;
  }
  else
  {
    unsigned long center = sum / counterInterval;
    unsigned long centerSum = center * counterInterval;
    unsigned long diffSum = sum - centerSum;
    
    Serial.print("Freq: ");
    Serial.print(center);
    Serial.print(".");
    Serial.print(diffSum);
    Serial.println("Hz");
    
    sum = 0;
    i = 0;
  }
  
  delay(1000);
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
