#ifndef FREQREADER_H
#define FREQREADER_H

#define rstPin 4
#define readyInt 1

// public member
extern volatile unsigned long freq;
void initFreqReader();

#endif
