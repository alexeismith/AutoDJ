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

#include "ThirdParty/soundtouch/include/SoundTouch.h"

class TrackProcessor
{
public:
    
    TrackProcessor(TrackDataManager* dm);
    
    ~TrackProcessor() {}
    
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    
    void load(TrackData track);
    
private:
    
    void processShift();
    
    TrackDataManager* dataManager = nullptr;
    
    std::atomic<bool> ready;
    
    TrackData currentTrack;
    
    juce::AudioBuffer<float> input;
    juce::AudioBuffer<float> output;
    
    int inputPlayhead = -1;
    int outputPlayhead = -1;
    
    soundtouch::SoundTouch shifter;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackProcessor)
};

#endif /* TrackProcessor_hpp */
