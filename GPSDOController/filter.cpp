#include <Arduino.h>
#include "common.h"
#include "filter.h"

// private
unsigned long records[MA_WINDOW];
unsigned long ma = 0;
int movingCount = 0;
int movingIndex = 0;

unsigned int range = 0; // percent
unsigned long upperBound = 0;
unsigned long lowerBound = 0;

void initMA()
{
  int i;
  
  movingCount = 0;
  movingIndex = 0;
  ma = 0;
  
  for (i = 0; i < MA_WINDOW; i++)
    records[i] = 0;
}

unsigned long movingAverage(unsigned long value)
{
  if (movingCount < MA_WINDOW)
    ma = (ma * movingCount + value) / (movingCount + 1);
  else
  {
    unsigned long oldestRecord = records[movingIndex];
    ma = ma + value / MA_WINDOW - oldestRecord / MA_WINDOW;
  }
    
  records[movingIndex] = value;
  
  movingIndex++;
  if (movingIndex == MA_WINDOW)
    movingIndex = 0;
  if (movingCount < MA_WINDOW)
    movingCount++;
    
  return ma;
}

void initRangeFilter(unsigned int range)
{
  Serial.println("initRangeFilter");
  
  if (range > 100)
    ::range = 100;
  else
    ::range = range;
  
  upperBound = centerFreq + centerFreq * (range / 100.0);
  lowerBound = centerFreq - centerFreq * (range / 100.0);
  Serial.print("Upper bound: ");
  Serial.println(upperBound);
  Serial.print("Lower bound: ");
  Serial.println(lowerBound);
}

unsigned long rangeFilter(unsigned long value)
{
  if (value <= upperBound && value >= lowerBound)
    return value;
  else
    return 0;
}

