//
//  WaveformLoader.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 06/05/2021.
//

#ifndef WaveformLoader_hpp
#define WaveformLoader_hpp

#include "WaveformScrollBar.hpp"

// NOTE: need a separate instance for each audio channel to be visualised, because the IIR filters depend on previous samples
class WaveformLoader : juce::Thread
{
public:
    
    WaveformLoader(WaveformComponent* waveform, WaveformScrollBar* scrollBar);
    
    ~WaveformLoader() { stopThread(3000); }
    
    void load(Track* track);
    
    void run();
    
private:
    
    void reset();
    
    void pushFrame(int index);
    
    WaveformComponent* waveform;
    WaveformScrollBar* scrollBar;
    
    Track* track;
    
    std::atomic<bool> loading = false;
    
    juce::AudioBuffer<float> processBuffers;
    
    juce::Array<juce::Colour> colours;
    juce::Array<float> levels;
    
    juce::IIRFilter filterLow, filterMid, filterHigh;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformLoader)
};

#endif /* WaveformLoader_hpp */
