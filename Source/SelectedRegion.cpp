#include "SelectedRegion.h"

// specPath has x-coordinates in terms of FFT frame, y-coordinates in terms of FFT freq bin
SelectedRegion::SelectedRegion(Path* specPath, int nHop, float sampleRate, int nfft)
:
nHop(nHop),
sampleRate(sampleRate),
nfft(nfft),
sampleRateParamIndex(0),
orderParamIndex(1),
cfParamIndex(2),
bwParamIndex(3),
nTransitionSamples(512)
{    
    // TODO: calculate beginning cf and bw
    regionMinSample = std::numeric_limits<float>::max();
    float hzForMinSample = 400.0;
    regionMaxSample = std::numeric_limits<float>::min();

    
    sampleByHz = new Path();
    Path::Iterator pi{*specPath};
    float frameStart = pi.x1;
    float binStart = pi.y1;
    float sampleStart = frameStart * nHop;
    float hzStart = (binStart / (float) nfft) * sampleRate;
    sampleByHz->startNewSubPath(sampleStart, hzStart);
    // no idea why this next needs to be there, without it, the first x1,y1 are both zero
    pi.next();
    while(pi.next()) {
        float frame = pi.x1;
        float bin = pi.y1;
        float sample = frame * nHop;
        float hz = (bin / (float) nfft) * sampleRate;
        sampleByHz->lineTo(sample, hz);
        if (sample < regionMinSample) {
            regionMinSample = sample;
            hzForMinSample = hz;
        }
        if (sample > regionMaxSample) {
            regionMaxSample = sample;
        }
    }
    sampleByHz->closeSubPath();
    delete specPath;
    
    bpf = new Dsp::SmoothedFilterDesign <Dsp::Butterworth::Design::BandPass <4>, 2, Dsp::DirectFormII> (nTransitionSamples);
    Dsp::Params params;
    params[sampleRateParamIndex] = sampleRate; // sample rate
    params[orderParamIndex] = 4; // order
    params[cfParamIndex] = hzForMinSample; // center frequency
    params[bwParamIndex] = 1000; // band width
    bpf->setParams (params);
}

void SelectedRegion::processBlock(AudioSampleBuffer* buffer, int sourcePlayhead, int sourceLength) {
    int n = buffer->getNumSamples();
    for (int bufferPlayhead = 0; bufferPlayhead < n; bufferPlayhead++) {
        float* channel0 = buffer->getWritePointer(0, bufferPlayhead);
        float* channel1 = buffer->getWritePointer(1, bufferPlayhead);
        float* channels[2] = {channel0, channel1};
        
        // update the filter parameters
        if (sourcePlayhead % nTransitionSamples == 0) {
            Array<float> freqIntersections = getFreqIntersections(sourcePlayhead);
            std::pair<float, float> minMaxHz = util::getMinMax(freqIntersections);
            updateFilterParams(minMaxHz.first, minMaxHz.second);
        }
        bpf->process(1, channels);
        if (!(regionMinSample < sourcePlayhead && sourcePlayhead < regionMaxSample)) {
            // playhead does not overlap with this region at the current sample
            buffer->setSample(0, bufferPlayhead, 0.0f);
            buffer->setSample(1, bufferPlayhead, 0.0f);
        } 
        sourcePlayhead = (sourcePlayhead + 1) % sourceLength;
    }
}

Array<float> SelectedRegion::getFreqIntersections(int sourcePlayhead) {
    float sample = sourcePlayhead;
    float top = sampleRate;
    float bottom = 0.0f;
    Line<float> sampleLine{sample, top, sample, bottom};
    return util::getYIntersections(sampleByHz, sampleLine);
}

void SelectedRegion::updateFilterParams(float minHz, float maxHz)
{
    float centerFreq = (minHz + maxHz) / 2.0f;
    float bandwidth = std::abs(minHz - maxHz);
    if (bandwidth <= 0.0f) {
        bandwidth = 1.0f;
    }
    //std::cout << "centerFreq: " << centerFreq << std::endl;
    //std::cout << "bandwidth: " << bandwidth << std::endl;
    bpf->setParam(cfParamIndex, centerFreq);
    bpf->setParam(bwParamIndex, bandwidth);
}
