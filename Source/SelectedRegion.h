#ifndef SELECTEDREGION_H_INCLUDED
#define SELECTEDREGION_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "DspFilters/dsp.h"
#include "util.h"
#include <limits>


class SelectedRegion
{
public:
    SelectedRegion(Path* specPath, int nHop, float sampleRate, int nfft);
    void processBlock(AudioSampleBuffer* buffer, int sourcePlayhead, int sourceLength);

private:
    Array<float> getFreqIntersections(int sourcePlayhead);
    void updateFilterParams(float minHz, float maxHz);
    
    const int nHop;
    const float sampleRate;
    const int nfft;
    const int sampleRateParamIndex;
    const int orderParamIndex;
    const int cfParamIndex;
    const int bwParamIndex;
    const int nTransitionSamples;
    
    float regionMinSample;
    float regionMaxSample;
    
    ScopedPointer<Path> sampleByHz;
    ScopedPointer<Dsp::Filter> bpf;
};


#endif  // SELECTEDREGION_H_INCLUDED
