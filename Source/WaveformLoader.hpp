//
//  WaveformLoader.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 06/05/2021.
//

#ifndef WaveformLoader_hpp
#define WaveformLoader_hpp

#include "WaveformScrollBar.hpp"
#include "DataManager.hpp"

// NOTE: need a separate instance for each audio channel to be visualised, because the IIR filters depend on previous samples
class WaveformLoader : juce::Thread
{
public:
    
    WaveformLoader(DataManager* dm, WaveformComponent* waveform, WaveformScrollBar* scrollBar, bool hideWhenEmpty);
    
    ~WaveformLoader() { stopThread(3000); }
    
    void load(Track* track, bool force);
    
    void run();
    
private:
    
    void process();
    
    void processFrame(int index);
    
    void reset();
    
    juce::CriticalSection lock;
    
    DataManager* dataManager = nullptr;
    
    WaveformComponent* waveform = nullptr;
    WaveformScrollBar* scrollBar = nullptr;
    
    Track track;
    Track newTrack;
    
    bool newRequest = false;
    
    bool loadedAudio = false;
    
    bool hideWhenEmpty;
    
    juce::AudioBuffer<float> processBuffers;
    
    juce::Array<juce::Colour> colours;
    juce::Array<float> levels;
    
    juce::IIRFilter filterLow, filterMid, filterHigh;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformLoader)
};

#endif /* WaveformLoader_hpp */
