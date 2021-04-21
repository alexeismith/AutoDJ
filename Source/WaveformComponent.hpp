//
//  WaveformComponent.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 08/04/2021.
//

#ifndef WaveformComponent_hpp
#define WaveformComponent_hpp

#define WAVEFORM_FRAME_SIZE (380)

#include <JuceHeader.h>

#include "TrackDataManager.hpp"

// NOTE: need a separate instance for each audio channel to be visualised, because the IIR filters depend on previous samples
class WaveformComponent : public juce::Component, juce::Timer
{
public:
    
    WaveformComponent(int width, TrackDataManager* dataManager);
    
    ~WaveformComponent() {}
    
    void paint(juce::Graphics& g) override;
    
    void resized() override { imageValid = false; }
    
    void loadTrack(TrackData track, int startSample = 0);
    
    void scroll(int samples);
    
    void timerCallback() override { stretch = -200 * (sin(juce::Time::getCurrentTime().toMilliseconds() * 0.001) + 1); repaint(); } //{ scroll(44100 / 30); }
    
private:
    
    void updateImage();
    
    void reset();
    
    void pushFrame(int index);
    
    bool isBeat(int frameIndex, bool& downbeat);
    
    TrackDataManager* dataManager;
    
    TrackData track;
    
    int numFrames = 0, startFrame, prevStartFrame, scrollRemainder = 0;
    
    bool initialised = false;
    
    juce::Image image, prevImage;
    bool imageValid = false;
    
    juce::AudioBuffer<float> processBuffers;
    
    juce::Array<juce::Colour> colours;
    juce::Array<float> levels;
    
    juce::IIRFilter filterLow, filterMid, filterHigh;
    
    int stretch = 0; //TODO: temp
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformComponent)
};

#endif /* WaveformComponent_hpp */
