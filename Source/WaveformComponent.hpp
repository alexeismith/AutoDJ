//
//  WaveformComponent.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 08/04/2021.
//

#ifndef WaveformComponent_hpp
#define WaveformComponent_hpp

//#define WAVEFORM_HALF_RESOLUTION

#ifdef WAVEFORM_HALF_RESOLUTION
    #define WAVEFORM_FRAME_SIZE (760)
#else
    #define WAVEFORM_FRAME_SIZE (380)
#endif

#include <JuceHeader.h>

#include "Track.hpp"

// NOTE: need a separate instance for each audio channel to be visualised, because the IIR filters depend on previous samples
class WaveformComponent : public juce::Component
{
public:
    
    WaveformComponent();
    
    ~WaveformComponent() {}
    
    void paint(juce::Graphics& g) override;
    
    void resized() override;
    
    void draw(int playhead, double timeStretch, double gain);
    
    void flipImage();
    
    void reset();
    
private:
    
    void loadTrack(Track* track);
    
    void updateImage();
    
    void pushFrame(int index);
    
    bool isBeat(int frameIndex, bool& downbeat);
    
    friend class WaveformLoadThread;
    
    Track* track;
    
    std::atomic<bool> ready;
    
    float brightness = 0.0;
    
    int numFrames = 0, startFrame, barHeight, drawWidth = 0;
    
    juce::OwnedArray<juce::Image> images;
    std::atomic<int> imageToPaint;
    
    juce::AudioBuffer<float> processBuffers;
    
    juce::Array<juce::Colour> colours;
    juce::Array<float> levels;
    
    juce::IIRFilter filterLow, filterMid, filterHigh;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformComponent)
};


class WaveformLoadThread : public juce::Thread
{
public:
    
    WaveformLoadThread(WaveformComponent* wave) :
        juce::Thread("WaveformLoad"), waveform(wave) {}
    
    ~WaveformLoadThread() { stopThread(3000); }
    
    void load(Track* t) { track = t; startThread(); }
    
    void run() { waveform->loadTrack(track); }
    
private:
    
    WaveformComponent* waveform = nullptr;
    
    Track* track = nullptr;
    
};

#endif /* WaveformComponent_hpp */
