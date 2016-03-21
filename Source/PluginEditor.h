#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "SelectedRegion.h"
#include "Util.h"
#include <map>
#include <ctime>
#include <limits>


//==============================================================================
class AudioLassoAudioProcessorEditor  :
public AudioProcessorEditor,
public FileDragAndDropTarget,
public Timer
{
public:
    AudioLassoAudioProcessorEditor (AudioLassoAudioProcessor&);
    ~AudioLassoAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    void filesDropped (const StringArray& files, int x, int y) override;
    bool isInterestedInFileDrag (const StringArray& files) override;
private:
    
    float pathWidth = 3.0f;
    int nFrames;
    int nFreqBins;
    
    Path* convertToSpecPath(Path* path);
    
    float getFrameForX(float x);
    float getBinForY(float y);

    void updateSpecImage();
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioLassoAudioProcessor& processor;
    
    ScopedPointer<Image> spectrogramImage;
    Path* drawPath;
    OwnedArray<Path> drawnPaths;
    
    void mouseDown(const MouseEvent &event) override;
    void mouseUp(const MouseEvent &event) override;
    void mouseDrag(const MouseEvent& event) override;
    
    void timerCallback() override;
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioLassoAudioProcessorEditor)
};



#endif  // PLUGINEDITOR_H_INCLUDED
