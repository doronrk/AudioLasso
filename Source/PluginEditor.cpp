#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
AudioLassoAudioProcessorEditor::AudioLassoAudioProcessorEditor (AudioLassoAudioProcessor& p)
    : AudioProcessorEditor (&p),
      processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    int width = 1300;
    int height = 700;
    spectrogramImage = new Image(Image::RGB, width, height, false);
    drawPath = new Path();
    setSize (width, height);
    startTimerHz(10);
}

AudioLassoAudioProcessorEditor::~AudioLassoAudioProcessorEditor()
{
}

//==============================================================================
void AudioLassoAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::black);
    g.setOpacity (1.0f);
    
    g.drawImageWithin (*spectrogramImage, 0, 0, getWidth(), getHeight(), RectanglePlacement::stretchToFit);
    
    // paint playhead progress
    float playheadProgress = processor.getPlayheadProgress();
    float playheadX = playheadProgress * getWidth();
    Line<float> playheadLine{playheadX, 0.0f, playheadX, (float)getHeight()};
    
    // paint the drawn paths
    for (Path* p: drawnPaths) {
        g.setColour(juce::Colour::fromRGB(25, 25, 250));
        g.strokePath(*p, PathStrokeType (pathWidth));
        
        Line<float> clipped = util::clipLineInsidePath(p, playheadLine);
        g.setColour(juce::Colour::fromRGB(25, 250, 25));
        g.drawLine(clipped);
    }

    // draw the draw path
    g.setColour(juce::Colour::fromRGB(250, 25, 25));
    g.strokePath(*drawPath, PathStrokeType (pathWidth));
}

void AudioLassoAudioProcessorEditor::updateSpecImage()
{
    SpecData* specData = processor.getSpecData();
    if (specData == nullptr) {
        return;
    }
    // paint the spectrogram itself
    float maxDb = 20*log10(specData->getMax());
    float** freqData = specData->getFreqData();
    nFrames = specData->getNFrames();
    nFreqBins = specData->getNFreqBins();
    
    int imageWidth = spectrogramImage->getWidth();
    int imageHeight = spectrogramImage->getHeight();

    
    for (int w = 0; w < imageWidth; w++) {
        for (int h = 1; h < imageHeight; h++) {
            int frame = getFrameForX(w);
            int fftDataIndex = getBinForY(h);
            float mag = freqData[frame][fftDataIndex];
            float magDb = 20*log10(mag);
            const float level = jmap(magDb, 0.0f, maxDb, 0.0f, 1.0f);
            spectrogramImage->setPixelAt (w, h, Colour::fromHSV (level, 1.0f, level, 1.0f));
        }
    }
}

void AudioLassoAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}


float AudioLassoAudioProcessorEditor::getFrameForX(float x) {
    float width = getWidth();
    return (x / width) * (float) nFrames;
}

float AudioLassoAudioProcessorEditor::getBinForY(float y) {
    float height = getHeight();
    const float skewedProportionY = 1.0f - std::exp (std::log (y / (float) height) * 0.2f);
    return jlimit (0.0f, nFreqBins / 2.0f,  (skewedProportionY * nFreqBins / 2.0f));
}

void AudioLassoAudioProcessorEditor::mouseDown(const MouseEvent &event)
{
    int x = event.x;
    int y = event.y;
    drawPath->clear();
    drawPath->startNewSubPath(x, y);
    repaint();
}

void AudioLassoAudioProcessorEditor::mouseDrag(const MouseEvent& event)
{
    int x = event.x;
    int y = event.y;
    drawPath->lineTo(x, y);
    repaint();
}

void AudioLassoAudioProcessorEditor::mouseUp(const MouseEvent &event)
{
    int x = event.x;
    int y = event.y;
    drawPath->lineTo(x, y);
    drawPath->closeSubPath();
    Path* specPath = convertToSpecPath(drawPath);
    processor.addSelectedRegion(specPath);
    drawnPaths.add(drawPath);
    drawPath = new Path();
    repaint();
}

Path* AudioLassoAudioProcessorEditor::convertToSpecPath(Path* path)
{
    float width = getWidth();
    float height = getHeight();
    Path* relPath = new Path();
    Path::Iterator pi{*path};
    float xAbsStart = pi.x1;
    float yAbsStart = pi.y1;
    float xRelStart = xAbsStart / width;
    float yRelStart = yAbsStart / height;
    relPath->startNewSubPath(xRelStart, yRelStart);
    while(pi.next()) {
        float xAbs = pi.x1;
        float yAbs = pi.y1;
        float frame = getFrameForX(xAbs);
        float bin = getBinForY(yAbs);
        relPath->lineTo(frame, bin);
    }
    relPath->closeSubPath();
    return relPath;
}

void AudioLassoAudioProcessorEditor::filesDropped (const StringArray& files, int x, int y)
{
    String file = files[0];
    processor.setAudioFile(file);
    updateSpecImage();
    repaint();
}

bool AudioLassoAudioProcessorEditor::isInterestedInFileDrag (const StringArray& files)
{
    String file = files[0];
    return processor.interestedInFile(file);
}

void AudioLassoAudioProcessorEditor::timerCallback()
{
    repaint();
}
