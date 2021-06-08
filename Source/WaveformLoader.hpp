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
    
    /** Constructor. */
    WaveformLoader(DataManager* dm, WaveformComponent* waveform, WaveformScrollBar* scrollBar, bool hideWhenEmpty);
    
    /** Destructor. */
    ~WaveformLoader() { stopThread(3000); }
    
    void load(Track* track, bool force);
    
    void run();
    
private:
    
    void process();
    
    void processFrame(int index);
    
    void reset();
    
    juce::CriticalSection lock; ///< RAII lock to ensure thread-safety while acessing data within this class
    
    DataManager* dataManager = nullptr; ///< Pointer to the app's track data manager
    
    WaveformComponent* waveform = nullptr; ///< Pointer to large waveform to receive the data loaded here
    WaveformScrollBar* scrollBar = nullptr; ///< Pointer to thin waveform to receive the data loaded here
    
    Track track;
    Track newTrack;
    
    bool newRequest = false;
    
    bool loadedAudio = false;
    
    bool hideWhenEmpty;
    
    juce::AudioBuffer<float> processBuffers;
    
    juce::Array<juce::Colour> colours;
    juce::Array<float> levels;
    
    juce::IIRFilter filterLow, filterMid, filterHigh;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformLoader) ///< JUCE macro to add a memory leak detector
};

#endif /* WaveformLoader_hpp */
