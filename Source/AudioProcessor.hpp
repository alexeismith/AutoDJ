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
    
    AudioProcessor(TrackDataManager* dataManager, ArtificialDJ* dj, int initBlockSize);
    
    ~AudioProcessor() {}
    
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    
    void play() { paused.store(false); }
    
    void play(TrackInfo track);
    
    void pause() { paused.store(true); }
    
    void preview(TrackInfo track, int startSample, int numSamples);
    
    TrackProcessor* getTrackProcessor(int index) { return trackProcessors.getUnchecked(index); }
    
    void getTrackProcessors(TrackProcessor** leader, TrackProcessor** next);
    
    TrackProcessor** getTrackProcessors() { return trackProcessors.data(); }
    
    void prepare(int blockSize);
    
private:
    
    std::atomic<bool> paused;
    
    juce::OwnedArray<TrackProcessor> trackProcessors;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioProcessor)
};

#endif /* AudioProcessor_hpp */
