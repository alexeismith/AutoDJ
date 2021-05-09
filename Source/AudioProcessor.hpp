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
    
    // We halve the volume, because two tracks could be playing at the same time
    // TODO: attenuate less and add a limiter?
    void setVolume(float newVolume) { targetVolume.store(newVolume/2.f); }
    
    void skip() { skipFlag.store(true); }
    
private:
    
    void skipToNextEvent();
    
    float volume = 0.5f;
    std::atomic<float> targetVolume = 0.5f;
    
    std::atomic<bool> skipFlag = false;
    
    std::atomic<bool> paused;
    
    juce::OwnedArray<TrackProcessor> trackProcessors;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioProcessor)
};

#endif /* AudioProcessor_hpp */
