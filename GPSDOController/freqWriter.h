#ifndef FREQWRITER_H
#define FREQWRITER_H

#define CALI_TIME 10 // in seconds
#define DAC_NO 2

// public
extern float maxFreq;
extern float minFreq;
extern float freqDist;
extern int valueDist;
extern float ratio;

void initFreqWriter();

// inaccurate, freq should be fine-tuned by steering system
void writeFreq(long freq);
// sterring
void steer(long freq, int count, int interval);
void writeCorr(float corr);

#endif

