

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "SpecHandler.h"
#include "SelectedRegion.h"


//==============================================================================
/**
*/
class AudioLassoAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    AudioLassoAudioProcessor();
    ~AudioLassoAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool silenceInProducesSilenceOut() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    SpecData* getSpecData();
    OwnedArray<SelectedRegion>& getSelectedRegions();
    float getPlayheadProgress();
    void addSelectedRegion(Path* p);

    bool setAudioFile(const String& audioFile);
    bool interestedInFile(const String& file);
private:
    
    ScopedPointer<AudioFormatManager> audioFormatManager;
    ScopedPointer<SpecHandler> specHandler;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioLassoAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
