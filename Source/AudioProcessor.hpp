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
    
    AudioProcessor(TrackDataManager* dataManager, ArtificialDJ* dj);
    
    ~AudioProcessor() {}
    
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    
    void play() { paused.store(false); }
    
    void play(TrackData track);
    
    void pause() { paused.store(true); }
    
    void preview(TrackData track, int startSample, int numSamples);
    
    TrackProcessor* getProcessor(int index) { return trackProcessors.getUnchecked(index); }
    
    void getProcessors(TrackProcessor** leader, TrackProcessor** next);
    
private:
    
    std::atomic<bool> paused;
    
    juce::OwnedArray<TrackProcessor> trackProcessors;
    
    std::unique_ptr<TrackProcessor> previewProcessor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioProcessor)
};

#endif /* AudioProcessor_hpp */
