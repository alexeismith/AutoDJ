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

#include "ArtificialDJ.hpp"

#include "ThirdParty/soundtouch/include/SoundTouch.h"

class TrackProcessor
{
public:
    
    TrackProcessor(TrackDataManager* dm);
    
    ~TrackProcessor() {}
    
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    
    void load(TrackData track);
    
    void seek(int sample) { inputPlayhead = sample; }
    
    void seekClip(int sample, int length);
    
    
private:
    
    void updateState();
    
    void reset();
    
    void processShifts(const juce::AudioSourceChannelInfo& bufferToFill);
    
    
    juce::CriticalSection lock;
    
    TrackDataManager* dataManager = nullptr;
    
    bool ready;
    
    TrackData currentTrack;
    std::unique_ptr<TrackState> trackState;
    
    juce::AudioBuffer<float> input;
    
    int inputLength;
    int inputPlayhead = -1;
    
    soundtouch::SoundTouch shifter;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackProcessor)
};

#endif /* TrackProcessor_hpp */
