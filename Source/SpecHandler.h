#ifndef SPECHANDLER_H_INCLUDED
#define SPECHANDLER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "SpecData.h"
#include "SelectedRegion.h"


class SpecHandler
{
public:
    SpecHandler();
    ~SpecHandler();
    
    void processBlock(AudioSampleBuffer& buffer);
    void setSourceAudio(AudioSampleBuffer* newSource, float sampleRate);
    float getPlayheadProgress();
    void resetPlayheadPosition();
    
    SpecData* getSpecData();
    OwnedArray<SelectedRegion>& getSelectedRegions();
    void addSelectedRegion(Path* p);
    

private:
    float pi;
    
    ScopedPointer<SpecData> specData;
    ScopedPointer<AudioSampleBuffer> sourceAudio;
    OwnedArray<SelectedRegion> selectedRegions;
    uint playheadPosition;
    
    const int nfft;
    const int windowSize;
    const int nHop;
    const float dbClip;
    
    float sampleRate;
};



#endif  // SPECHANDLER_H_INCLUDED
