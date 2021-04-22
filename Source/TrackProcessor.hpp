//
//  TrackProcessor.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 19/04/2021.
//

#ifndef TrackProcessor_hpp
#define TrackProcessor_hpp

#include <JuceHeader.h>

#include "TrackDataManager.hpp"

#include "TrackState.hpp"

#include "ThirdParty/soundtouch/include/SoundTouch.h"

class ArtificialDJ;

class TrackProcessor
{
public:
    
    TrackProcessor(TrackDataManager* dm, ArtificialDJ* dj);
    
    ~TrackProcessor() {}
    
    bool getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill, bool play = true);
    
    void loadTrack();
    
    void seek(int sample) { inputPlayhead = sample; }
    
    void seekClip(int sample, int length);
    
    TrackState* getState() { return state.get(); }
    
    bool isLeader() { return getState()->leader; }
    
    void updateState();
    
private:
    
    void reset();
    
    void processShifts(const juce::AudioSourceChannelInfo& bufferToFill);
    
    
    juce::CriticalSection lock;
    
    TrackDataManager* dataManager = nullptr;
    
    bool ready;
    
    std::unique_ptr<TrackState> state;
    
    juce::AudioBuffer<float> input;
    
    int inputLength;
    int inputPlayhead = -1;
    
    soundtouch::SoundTouch shifter;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackProcessor)
};

#endif /* TrackProcessor_hpp */
