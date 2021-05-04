//
//  WaveformBarComponent.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 04/05/2021.
//

#ifndef WaveformBarComponent_hpp
#define WaveformBarComponent_hpp

#include "WaveformComponent.hpp"

#define WAVEFORM_BAR_HEIGHT (30)

// NOTE: must be same width as the WaveformComponent its paired to, in order for windowed section to be correct
class WaveformBarComponent : public WaveformComponent
{
public:
    
    WaveformBarComponent();
    
    ~WaveformBarComponent() {}
    
    void paint(juce::Graphics& g) override;
    
    void resized() override;
    
    void update(int playhead, double timeStretch = 1.0, double gain = 1.0) override;
    
private:
    
    void draw() override;
    
    bool isBeat(int frameIndex, bool& downbeat) override;
    
    int windowStartX, windowWidth;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformBarComponent)
};


class WaveformLoadThread : public juce::Thread
{
public:
    
    WaveformLoadThread() : juce::Thread("WaveformLoad") {}
    
    ~WaveformLoadThread() { stopThread(3000); }
    
    void load(WaveformComponent* waveform, WaveformBarComponent* bar, Track* t);
    
    void run() { waveform->loadTrack(track); bar->loadTrack(track); }
    
private:
    
    WaveformComponent* waveform = nullptr;
    WaveformBarComponent* bar = nullptr;
    Track* track = nullptr;
    
};

#endif /* WaveformBarComponent_hpp */
