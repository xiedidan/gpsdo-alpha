#include <Arduino.h>
#include "common.h"
#include "filter.h"
#include "freqReader.h"
#include "dacWriter.h"
#include "freqWriter.h"

// public
float maxFreq = 0;
float minFreq = 0;
float freqDist = 0;
int valueDist = DAC_MAX_VALUE - DAC_MIN_VALUE;
float ratio = 0;
int value = 0;

// private
int convertFreq(long freq);
int getCorrection(long freqSum, int interval);

void initFreqWriter()
{
  Serial.println("initFreqWriter");
  int i;
  long freqSum;
  
  initRangeFilter(5);
  initDacWriter(); 
  delay(3000);
  
  // write max voltage
  writeDac(DAC_NO, DAC_MAX_VALUE);
  delay(2000);
  
  // read max freq
  freqSum = 0;
  i = 0;
  while (i < CALI_TIME)
  {
    delay(1000);
    if (rangeFilter(freq) > 0)
    {
      Serial.println(freq);
      freqSum += freq;
      i++;
    }
  }
  if (freqSum > centerFreq * CALI_TIME)
    maxFreq = (freqSum - centerFreq * CALI_TIME) / (float)CALI_TIME;
  else
    maxFreq = -1.0 * (centerFreq * CALI_TIME - freqSum) / (float)CALI_TIME;
  Serial.print("maxFreq: ");
  Serial.println(maxFreq, 16);
  
  // write min voltage
  writeDac(DAC_NO, DAC_MIN_VALUE);
  delay(2000);
  
  // read min freq
  freqSum = 0;
  i = 0;
  while (i < CALI_TIME)
  {
    delay(1000);
    if (rangeFilter(freq) > 0)
    {
      Serial.println(freq);
      freqSum += freq;
      i++;
    }
  }
  if (freqSum > centerFreq * CALI_TIME)
    minFreq = (freqSum - centerFreq * CALI_TIME) / (float)CALI_TIME;
  else
    minFreq = -1.0 * (centerFreq * CALI_TIME - freqSum) / (float)CALI_TIME;
  Serial.print("minFreq: ");
  Serial.println(minFreq, 16);
  
  freqDist = maxFreq - minFreq;
  ratio = freqDist / valueDist;
  Serial.print("DAC ratio: ");
  Serial.println(ratio, 16);
}

void writeFreq(long freq)
{
  int i;
  float freqCorr = 0;
  long freqSum = 0;
  value = convertFreq(freq);
  writeDac(DAC_NO, value);  
  delay(2000);
  
  i = 0;
  while (i < CALI_TIME)
  {
    delay(1000);
    if (rangeFilter(::freq) > 0)
    {
      freqSum += ::freq;
      i++;
    }
  }
  if (centerFreq * CALI_TIME > freqSum)
    freqCorr = (centerFreq * CALI_TIME - freqSum) / (float)CALI_TIME;
  else
    freqCorr = -1.0 * (freqSum - centerFreq * CALI_TIME) / (float)CALI_TIME;
  Serial.print("Freq Correction: ");
  Serial.println(freqCorr);
  
  float freqDist = maxFreq - minFreq;
  int valueDist = DAC_MAX_VALUE - DAC_MIN_VALUE;
  
  float ratio = 0.0;   
  ratio = freqDist / valueDist;
  
  int correction = freqCorr / ratio;
  
  value += correction;
  writeDac(DAC_NO, value);
  Serial.print("Refined DAC value: ");
  Serial.println(value);
}

void steer(long freq, int count, int interval)
{
  int i;
  int j;
  long freqSum = 0;
  int correction = 0;
  
  /*
  value = convertFreq(freq);
  writeDac(DAC_NO, value);  
  delay(2000);
  */
  
  j = 0;
  while(j < count)
  {
    Serial.println();
    i = 0;
    freqSum = 0;
    while(i < interval)
    {
      delay(1000);
      if (rangeFilter(::freq) > 0)
      {
        freqSum += ::freq;
        i++;
      }
    }
    
    correction = getCorrection(freqSum, interval);
    value += correction;
    Serial.print("Refined DAC value: ");
    Serial.println(value);
    writeDac(DAC_NO, value);
    delay(2000);
    
    j++;
  }
}

void writeCorr(float corr)
{
  int correction = corr / ratio;
  Serial.print("Correction: ");
  Serial.println(correction);
  
  value += correction;
  writeDac(DAC_NO, value);
  
  Serial.print("Refined DAC value: ");
  Serial.println(value);
}

int convertFreq(long freq)
{
  int value;
  
  // minFreq should be < 0
  value = DAC_MIN_VALUE - minFreq / ratio;
  Serial.print("DAC value: ");
  Serial.println(value);
  
  return value;
}

int getCorrection(long freqSum, int interval)
{
  float freqCorr = 0.0;
  
  if (centerFreq * interval > freqSum)
    freqCorr = (centerFreq * interval - freqSum) / (float)interval;
  else
    freqCorr = -1.0 * (freqSum - centerFreq * interval) / (float)interval;
    
  Serial.print("Freq Correction: ");
  Serial.println(freqCorr);
  
  int correction = freqCorr / ratio;
  
  Serial.print("DAC Correction: ");
  Serial.println(correction);
  
  return correction;
}

