#include "SpecData.h"

SpecData::SpecData(const float* samples, int nSamples, int nfft, float fs, Array<float>* window,
                   int nHop, float dbClip)
{
    std::cout << "top of SpecData constructor" << std::endl;
    ScopedPointer<FFT> forwardFFT = new FFT((int)log2(nfft), false);

    int m = window->size();
    int mOdd = m%2;
    int Mo2 = (m - mOdd)/2;
    nFrames = 1 + ceil((float) nSamples/nHop);
    nFreqBins = nfft;
    
    freqData = new float*[nFrames];
    
    int zp = nfft-m;
    float* xframe = new float[m];
    float* xWithPadding = new float[nfft];
    int xoff = 0 - Mo2;

    max = std::numeric_limits<float>::min();
    min = std::numeric_limits<float>::max();
    std::cout << "nFrames: " << nFrames << std::endl;
    for (int frame = 0; frame < nFrames; frame++) {
        if (frame % 100 == 0) {
            std::cout << "frame: " << frame << std::endl;
        }
        // populate xframe
        // clear xframe
        for (int i = 0; i < m; i++) {
            xframe[i] = 0;
        }
        if (xoff < 0) {
            memcpy(xframe, samples, sizeof(float) * (xoff + m));
        } else {
            if (xoff+m > nSamples) {
                int remaining = nSamples - xoff;
                memcpy(xframe, &samples[xoff], sizeof(float) * remaining);
                // fill remaining xframe with zeros
                for (int i = remaining; i < m; i++) {
                    xframe[i] = 0;
                }
            } else {
                memcpy(xframe, &samples[xoff], sizeof(float) * m);
            }
        }
        
        // apply the window
        FloatVectorOperations::multiply(xframe, window->begin(), m);
        // zero pad
        int middle = Mo2+1;
        memcpy(xWithPadding, &xframe[middle], (m-middle)*sizeof(float));
        int offset = m-middle;
        for (int i = 0; i < zp; i++) {
            xWithPadding[offset + i] = 0;
        }
        memcpy(&xWithPadding[offset + zp], xframe, (Mo2+1)*sizeof(float));
        forwardFFT->performFrequencyOnlyForwardTransform(xWithPadding);
        
        Range<float> maxLevel = FloatVectorOperations::findMinAndMax (xWithPadding, nfft);
        float maxCandidate = maxLevel.getEnd();
        if (maxCandidate > max) {
            max = maxCandidate;
        }
        float minCandidate = maxLevel.getStart();
        if (minCandidate < min) {
            min = minCandidate;
        }
        
        float* frameData = new float[nfft];
        memcpy(frameData, xWithPadding, sizeof(float) * nfft);
        freqData[frame] = frameData;
        
        xoff = xoff + nHop;
    }
    delete xframe;
    delete xWithPadding;
}

SpecData::~SpecData() {
    std::cout << "~SpecData" << std::endl;

}

float** SpecData::getFreqData() {
    return freqData;
}

int SpecData::getNFrames() {
    return nFrames;
}

int SpecData::getNFreqBins() {
    return nFreqBins;
}


float SpecData::getMax() {
    return max;
}

float SpecData::getMin() {
    return min;
}








