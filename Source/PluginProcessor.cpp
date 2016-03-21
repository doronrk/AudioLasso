#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
AudioLassoAudioProcessor::AudioLassoAudioProcessor()
{
    specHandler = new SpecHandler();
    audioFormatManager = new AudioFormatManager();
    audioFormatManager->registerBasicFormats();
    
}

AudioLassoAudioProcessor::~AudioLassoAudioProcessor()
{
}

//==============================================================================
const String AudioLassoAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioLassoAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioLassoAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioLassoAudioProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

double AudioLassoAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioLassoAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioLassoAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioLassoAudioProcessor::setCurrentProgram (int index)
{
}

const String AudioLassoAudioProcessor::getProgramName (int index)
{
    return String();
}

void AudioLassoAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void AudioLassoAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void AudioLassoAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void AudioLassoAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    buffer.clear();
    specHandler->processBlock(buffer);
}

//==============================================================================
bool AudioLassoAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* AudioLassoAudioProcessor::createEditor()
{
    return new AudioLassoAudioProcessorEditor (*this);
}

//==============================================================================
void AudioLassoAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AudioLassoAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}



//==============================================================================
bool AudioLassoAudioProcessor::setAudioFile(const String& audioFileAbsPath) {
    ScopedPointer<File> audioFile = new File(audioFileAbsPath);
    if (!audioFile->existsAsFile()) {
        std::cout << "!audioFile->existsAsFile()" << std::endl;
        return false;
    }
    ScopedPointer<AudioFormatReader> audioFormatReader = audioFormatManager->createReaderFor(*audioFile);
    int numChannels = audioFormatReader->numChannels;
    int numSamples = audioFormatReader->lengthInSamples;
    float sampleRate = audioFormatReader->sampleRate;
    AudioSampleBuffer* sourceAudio = new AudioSampleBuffer(numChannels, numSamples);
    audioFormatReader->read(sourceAudio, 0, numSamples, 0, true, true);
    
    const CriticalSection& lock = getCallbackLock();
    lock.enter();
    specHandler->setSourceAudio(sourceAudio, sampleRate);
    lock.exit();
    return true;
}



SpecData* AudioLassoAudioProcessor::getSpecData()
{
    return specHandler->getSpecData();
}

OwnedArray<SelectedRegion>& AudioLassoAudioProcessor::getSelectedRegions()
{
    return specHandler->getSelectedRegions();
}

float AudioLassoAudioProcessor::getPlayheadProgress()
{
    return specHandler->getPlayheadProgress();
}

void AudioLassoAudioProcessor::addSelectedRegion(Path* specPath)
{
    const CriticalSection& lock = getCallbackLock();
    
    lock.enter();
    specHandler->addSelectedRegion(specPath);
    lock.exit();
}

bool AudioLassoAudioProcessor::interestedInFile(const String& file)
{
    ScopedPointer<AudioFormatReader> afr = audioFormatManager->createReaderFor(file);
    return afr != nullptr;
}




//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioLassoAudioProcessor();
}
