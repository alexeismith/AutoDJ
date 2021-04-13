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
    
    void loadTrack(TrackData track, int startSample);
    
    void scroll(int samples);
    
    void timerCallback() override { scroll(44100 / 60); }
    
//    void analyse(juce::AudioBuffer<float> audio);
    
private:
    
    void reset();
    
    void pushFrame(int index);
    
    bool isBeat(int frameIndex);
    
    TrackDataManager* dataManager;
    
    TrackData track;
    
    int numFrames = 0, startFrame, scrollRemainder = 0;
    
    juce::AudioBuffer<float> processBuffers;
    
    juce::Array<juce::Colour> colours;
    juce::Array<float> levels;
    
    juce::IIRFilter filterLow, filterMid, filterHigh;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformComponent)
};

#endif /* WaveformComponent_hpp */
