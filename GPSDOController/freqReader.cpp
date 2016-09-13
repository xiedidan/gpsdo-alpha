#include <Arduino.h>
#include "softSPI.h"
#include "freqReader.h"

// public member
volatile unsigned long freq;

// private member
void readyISR();

void initFreqReader()
{
  initSoftSPI();
  
  pinMode(rstPin, OUTPUT);
  digitalWrite(rstPin, HIGH);
  delay(10);
  digitalWrite(rstPin, LOW);
  delay(10);
  digitalWrite(rstPin, HIGH);
  
  attachInterrupt(readyInt, readyISR, RISING);
}

void readyISR()
{
  freq = readSoftSPILong();
  // Serial.print("readyISR: ");
  // Serial.println(freq);
}
