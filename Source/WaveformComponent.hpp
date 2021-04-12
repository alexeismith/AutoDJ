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

#include "TrackData.hpp"

// NOTE: need a separate instance for each audio channel to be visualised, because the IIR filters depend on previous samples
class WaveformComponent : public juce::Component
{
public:
    
    WaveformComponent(int width);
    
    ~WaveformComponent() {}
    
    void paint(juce::Graphics& g) override;
    
    void prepare(TrackData track);
    
    void pushBuffer(const float* audio, int numSamples);
    
private:
    
    void reset();
    
    void pushFrame(const float* audio);
    
    bool isBeat(int frameIndex);
    
    TrackData track;
    
    juce::AudioBuffer<float> processBuffers;
    
    juce::Array<juce::Colour> colours;
    juce::Array<float> levels;
    
    juce::IIRFilter filterLow, filterMid, filterHigh;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformComponent)
};

#endif /* WaveformComponent_hpp */
