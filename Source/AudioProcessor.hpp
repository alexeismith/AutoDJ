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


/**
 Top-level audio processor, which handles playback control and master DSP.
 Track-specific DSP is delegated to the TrackProcessor children.
 This class receives the audio output buffer from MainComponent.
 */
class AudioProcessor
{
public:
    
    /** Constructor. */
    AudioProcessor(DataManager* dataManager, ArtificialDJ* dj, int initBlockSize);
    
    /** Destructor. */
    ~AudioProcessor() {}
    
    /** Audio processing loop - applies master DSP and delegates track-specific DSP to TrackProcessor children.
    
     @param[out] outputBuffer Buffer to fill with desired audio output samples
    
     @see TrackProcessor::getNextAudioBlock */
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& outputBuffer);
    
    /** Resumes audio playback. */
    void play() { paused.store(false); }
    
    /** Pauses audio playback. */
    void pause() { paused.store(true); }
    
    /** Fetches a child TrackProcessor.
     
     @param[in] index Array index of the TrackProcessor to fetch (position in the trackProcessors array)
     
     @return Pointer to the specified TrackProcessor */
    TrackProcessor* getTrackProcessor(int index) { return trackProcessors.getUnchecked(index); }
    
    /** Fetches both TrackProcessor children, determining which is currently leading the mix.
     The double pointers allow the calling function to receive TrackProcessor pointers into its local scope.
     
     @param[out] leader Double pointer to the leading TrackProcessor
     @param[out] follower Double pointer to the following TrackProcessor */
    void getTrackProcessors(TrackProcessor** leader, TrackProcessor** follower);
    
    /** Fetches an array of pointers to the TrackProcessor children.
     
     @return Array of pointers to TrackProcessors */
    TrackProcessor** getTrackProcessors() { return trackProcessors.data(); }
    
    /** Prepares the processing pipeline for a given audio buffer size.
     
     @param[in] blockSize Number of audio samples to expect in each processing block */
    void prepare(int blockSize);
    
    /** Sets the master volume level.
     The input argument is halved, because two normalised tracks could be playing at the same time.
     
     @param[in] newVolume Volume gain to apply at the audio output */
    void setVolume(float newVolume) { targetVolume.store(newVolume/2.f); }
    
    /** Requests a skip to the next mix event.
     A flag is set here, which will trigger the skip during the next audio processing loop. */
    void skip() { skipFlag.store(true); }
    
    /** Checks whether the DJ performance has finished.
     
     @return Result of check */
    bool mixEnded();
    
    /** Resets the audio processing pipeline, ready to start a new performance. */
    void reset();
    
private:
    
    /** Skips to the next event in the mix.
     If there is a transition in progress, the net even is the end of the transition.
     Otherwise, it is the start of the next transition. */
    void skipToNextEvent();
    
    float volume = 0.5f; ///< Current volume gain applied at the output
    std::atomic<float> targetVolume = 0.5f; ///< Target volume gain to ramp towards during the next audio processing loop
    
    std::atomic<bool> skipFlag = false; ///< Flag to trigger a skip to the next mix event
    
    std::atomic<bool> paused = true; ///< Flag to track the audio playback state
    
    juce::OwnedArray<TrackProcessor> trackProcessors; ///< Array of child TrackProcessors
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessor) ///< JUCE macro to add a memory leak detector
};

#endif /* AudioProcessor_hpp */
