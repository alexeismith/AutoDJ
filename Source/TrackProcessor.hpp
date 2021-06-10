//
//  TrackProcessor.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 19/04/2021.
//

#ifndef TrackProcessor_hpp
#define TrackProcessor_hpp

#include "DataManager.hpp"
#include "TimeStretcher.hpp"
#include "Track.hpp"
#include "ThirdParty/soundtouch/include/SoundTouch.h"

class ArtificialDJ;
class TrackLoadThread;


/**
 Audio processor for track-specific DSP.
 */
class TrackProcessor
{
public:
    
    /** Constructor. */
    TrackProcessor(DataManager* dm, ArtificialDJ* DJ);
    
    /** Destructor. */
    ~TrackProcessor() {}
    
    /** Audio processing loop - applies mixing parameters.
    
     @param[out] outputBuffer Buffer to fill with desired audio output samples */
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& outputBuffer);
    
    /** Fetches the Track data structure.
     This contains a pointer to the TrackInfo for the currently loaded track,
     as well as its playhead position and a pointer to audio data.
     
     @return Pointer to Track data structure for this processor */
    Track* getTrack() { return track.get(); }
    
    /** Checks whether this processor is leading the mix.
     
     @return Result of the check */
    bool isLeader() { return getTrack()->leader; }
    
    /** Fetches the next mixing transition and prepares the track state for it. */
    void nextMix();
    
    /** Checks whether this processor is ready for playback.
     
     @return Result of the check */
    bool isReady() { return ready.load(); }
    
    /** Checks whether the mix has ended and the current track has finished playing.
     
     @return Result of the check  */
    bool mixEnded() { return mixEnd.load(); } 
    
    /** Loads the next track information and gets ready for playback. */
    void loadNextTrack();
    
    /** Loads the first track information and gets ready to start the DJ mix.
     
     @param[in] trackInfo Pointer to the first track to be played by this processor
     @param[in] leader Indicates whether this will be the first processor to play
     @param[in] audio Pointer to the audio data for the first track */
    void loadFirstTrack(TrackInfo* trackInfo, bool leader, juce::AudioBuffer<float>* audio = nullptr);
    
    /** Prepares the processing pipeline for a given audio buffer size.
    
    @param[in] blockSize Number of audio samples to expect in each processing block */
    void prepare(int blockSize);
    
    /** Sets the companion instance of TrackProcessor
     
     @param[in] p Pointer to the other instance of TrackProcessor */
    void setPartner(TrackProcessor* p) { partner = p; }
    
    /** Fetches the time stretch factor currently applied to the track.
     
     @return Time stretch factor */
    double getTimeStretch() { return stretcher->getTimeStretch(); }
    
    /** Fetches the current mix information.
     
     @return Information on the current/next transition */
    MixInfo getCurrentMix() { return currentMix; }
    
    /** Fetches the current position in the track, in samples.
     
     @return Current playhead position */
    int getPlayheadPosition() { return track->getPlayhead(); }
    
    /** Synchronises the start of this processor with the provided leader playhead position.
     Only used when this is the following TrackProcessor.
     
     @param[in] leaderPlayhead Playhead position of the leading track */
    void cue(int leaderPlayhead);
    
    /** Skips the playhead position to the next mixing event. */
    void skipToNextEvent();
    
    /** Resets the processor ready for a new performance. */
    void reset();
    
private:
    
    /** Sets the frequency of the high-pass crossfade filters.
     
     @param[in] frequency Cut-off frequency for filters */
    void setHighPass(int frequency);
    
    /** Updates the state of the processor, based on how many samples have been processed.
     
     @param[in] numSamples Number of audio samples processed since last update */
    void update(int numSamples);
    
    /** Resets the track playhead position (back to the start by default).
     
     @param[in] sample Audio sample to set playhead to */
    void resetPlayhead(int sample = 0);
    
    /** Fetches the length of the loaded track.
     
     @return Length of the track in audio samples */
    int getAudioLength() { return track->audio->getNumSamples(); }
    
    
    juce::CriticalSection lock; ///< RAII lock to ensure thread-safety while acessing data within this class

    TrackProcessor* partner = nullptr; ///< Pointer to the companion instance of TrackProcessor
    DataManager* dataManager = nullptr; ///< Pointer to the app's track data manager
    ArtificialDJ* dj = nullptr; ///< Pointer to artificial DJ brain
    
    std::atomic<bool> ready; ///< Indicates whether the processor has a track loaded ready for playback
    
    bool play = false; ///< Tracks whether audio playback is running
    
    bool trackEnd = false; ///< Indicates whether the current track has finished playing
    std::atomic<bool> mixEnd = false; ///< Indicates whether the mix has ended and the current track has finished playing
    
    std::unique_ptr<Track> track; ///< Data on the current track and its playback state
    MixInfo currentMix; ///< Parameters of current/next mix transition
    
    juce::AudioBuffer<float> processBuffer; ///< Intermediate audio buffer, which is copied into the output buffer after processing
    
    std::unique_ptr<TimeStretcher> stretcher; ///< Handles time stretching of track audio

    // IIR filters depend on previous samples, so we can't switch between different audio channels during processing
    // This means two high pass filters are needed - one for each channel
    juce::IIRFilter highPassFilterL; ///< IIR filter for high-passing the left audio channel
    juce::IIRFilter highPassFilterR; ///< IIR filter for high-passing the right audio channel
    bool filterOn = false; ///< Tracks whether high-pass filtering should be applied
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackProcessor) ///< JUCE macro to add a memory leak detector
};

#endif /* TrackProcessor_hpp */
