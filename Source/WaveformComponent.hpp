//
//  WaveformComponent.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 08/04/2021.
//

#ifndef WaveformComponent_hpp
#define WaveformComponent_hpp

#include <JuceHeader.h>

#include "Track.hpp"

#define WAVEFORM_HEIGHT (100)

#define WAVEFORM_FRAME_SIZE (380)

// NOTE: need a separate instance for each audio channel to be visualised, because the IIR filters depend on previous samples
class WaveformComponent : public juce::Component
{
public:
    
    WaveformComponent();
    
    ~WaveformComponent() {}
    
    virtual void paint(juce::Graphics& g) override;
    
    void resized() override;
    
    virtual void draw(int playhead, double timeStretch, double gain);
    
    void flipImage();
    
    void reset();
    
    
protected:
    
    void updateImage();
    
    virtual bool isBeat(int frameIndex, bool& downbeat);
    
    std::atomic<bool> ready;
    
    float brightness = 0.0;
    
    int frameSize, numFrames = 0, startFrame = 0, drawWidth = 0;
    
private:
    
    void loadTrack(Track* track);
    
    void pushFrame(int index);
    
    friend class WaveformLoadThread;
    
    Track* track;
    
    int barHeight;
    
    juce::OwnedArray<juce::Image> images;
    std::atomic<int> imageToPaint;
    
    juce::AudioBuffer<float> processBuffers;
    
    juce::Array<juce::Colour> colours;
    juce::Array<float> levels;
    
    juce::IIRFilter filterLow, filterMid, filterHigh;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformComponent)
};

#endif /* WaveformComponent_hpp */
