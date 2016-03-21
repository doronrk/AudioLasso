#ifndef SPECDATA_H_INCLUDED
#define SPECDATA_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class SpecData
{
public:
    SpecData(const float* samples, int nSamples, int nfft, float fs, Array<float>* window,
             int nOverlap, float dbClip);
    
    ~SpecData();
    
    float** getFreqData();
    int getNFrames();
    int getNFreqBins();
    float getMax();
    float getMin();
    
private:
    float** freqData;
    int nFrames;
    int nFreqBins;
    float max;
    float min;
};


#endif  // SPECDATA_H_INCLUDED
