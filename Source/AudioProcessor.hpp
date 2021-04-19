//
//  AudioProcessor.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 09/04/2021.
//

#ifndef AudioProcessor_hpp
#define AudioProcessor_hpp

#include <JuceHeader.h>

#include "TrackProcessor.hpp"

class AudioProcessor
{
public:
    
    AudioProcessor(TrackDataManager* dataManager);
    
    ~AudioProcessor() {}
    
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    
    void play(TrackData track);
    
    
private:
    
    juce::OwnedArray<TrackProcessor> trackProcessors;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioProcessor)
};

#endif /* AudioProcessor_hpp */
