#include "SpecHandler.h"
SpecHandler::SpecHandler()
:
pi(3.14159265359),
playheadPosition(0),
nfft(2048),
windowSize(512),
nHop(256),
dbClip(100)
{
    sourceAudio = new AudioSampleBuffer();
}

SpecHandler::~SpecHandler() {
    std::cout << "~SpecHandler" << std::endl;
}

void SpecHandler::processBlock(AudioSampleBuffer& buffer)
{
    if (sourceAudio->getNumSamples() == 0) {
        return;
    }
    // copy data from the source audio to beforeFiltering
    ScopedPointer<AudioSampleBuffer> beforeFiltering = new AudioSampleBuffer(buffer.getNumChannels(), buffer.getNumSamples());
    beforeFiltering->clear();
    int samplesRemaining = sourceAudio->getNumSamples() - playheadPosition;
    int nBeforeWrap = std::min(samplesRemaining, buffer.getNumSamples());
    int nAfterWrap = buffer.getNumSamples() - nBeforeWrap;
    for (int destChannel = 0; destChannel < beforeFiltering->getNumChannels(); destChannel++) {
        int sourceChannel = destChannel % sourceAudio->getNumChannels();
        beforeFiltering->copyFrom(destChannel, 0, *sourceAudio, sourceChannel, playheadPosition, nBeforeWrap);
        beforeFiltering->copyFrom(destChannel, nBeforeWrap, *sourceAudio, sourceChannel, 0, nAfterWrap);
    }
    
    int srSize = selectedRegions.size();
    // add the filtered results to the output buffer
    for (SelectedRegion* sr: selectedRegions) {
        ScopedPointer<AudioSampleBuffer> beforeFilterCopy = new AudioSampleBuffer(*beforeFiltering);
        sr->processBlock(beforeFilterCopy, playheadPosition, sourceAudio->getNumSamples());
        
        Range<float> minMax1 = beforeFilterCopy->findMinMax(0, 0, beforeFilterCopy->getNumSamples());
        Range<float> minMax2 = beforeFilterCopy->findMinMax(1, 0, beforeFilterCopy->getNumSamples());
        
//        std::cout << "min1: " << minMax1.getStart() << std::endl;
//        std::cout << "max1: " << minMax1.getEnd() << std::endl;
//        std::cout << "min2: " << minMax2.getStart() << std::endl;
//        std::cout << "max2: " << minMax2.getEnd() << std::endl;

        for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
            buffer.addFrom(channel, 0, *beforeFilterCopy, channel, 0, beforeFilterCopy->getNumSamples());
        }
    }
    
    //buffer.makeCopyOf(*beforeFiltering);

    playheadPosition = (playheadPosition + buffer.getNumSamples()) % sourceAudio->getNumSamples();
}

void SpecHandler::setSourceAudio(AudioSampleBuffer* newSource, float fs)
{
    std::cout << "top of setSourceAudio in SpecHandler" << std::endl;
    // TODO: lock audiocallback thread
    sourceAudio = newSource;
    const float* samples = sourceAudio->getReadPointer(0);
    int nSamples = sourceAudio->getNumSamples();
    sampleRate = fs;
    
    Array<float>* window = new Array<float>();
    window->resize(windowSize);
    for (int i = 0; i < windowSize; i++) {
        float amp = 0.54 - 0.46*cos(2*pi*((float) i / windowSize));
        window->set(i, amp);
    }
    
    specData = new SpecData(samples, nSamples, nfft, sampleRate, window, nHop, dbClip);
    // TODO: clear selected regions
    // TODO: stop playback
    // TODO: reset playhead
    // TODO: redraw editor
    std::cout << "finished initializing specData" << std::endl;
}


float SpecHandler::getPlayheadProgress() {
    return playheadPosition/ (float) sourceAudio->getNumSamples();
}

void SpecHandler::resetPlayheadPosition() {
    playheadPosition = 0;
}

SpecData* SpecHandler::getSpecData() {
    return specData;
}

OwnedArray<SelectedRegion>& SpecHandler::getSelectedRegions()
{
    return selectedRegions;
}

void SpecHandler::addSelectedRegion(Path* specPath)
{
    SelectedRegion* sr = new SelectedRegion(specPath, nHop, sampleRate, nfft);
    selectedRegions.add(sr);
}


