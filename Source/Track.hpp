//
//  Track.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 21/04/2021.
//

#ifndef Track_hpp
#define Track_hpp

#include "InterpolatedParameter.hpp"
#include "MixInfo.hpp"


#define HIGH_PASS_MAX (150) ///< Peak cut-off frequency of high-pass filtering during a crossfade


/**
 Holds data on the playback state of a track, as well as pointers to the general track information and its audio data.
 */
class Track
{
public:
    
    /** Constructor. */
    Track() {}
    
    /** Destructor. */
    ~Track() {}
    
    /** Updates the track state based on the number of audio samples that have been processed.
     
     @param[in] numSamples Number of samples processed since last update
     
     @return True if the current transition has finished */
    bool update(int numSamples);
    
    /** Applies the provided mix information to the track state, readying it for the next transition.
     
     @param[in] mix Information on the next transition
     
     @return False if there is no track to transition to */
    bool applyNextMix(MixInfo* mix);
    
    /** Fetches the current playhead position of the track.
     
     @return Current track position, in audio samples */
    int getPlayhead() { return playhead; }
    
    /** Resets the track playhead to a given position.
    
    @param[in] sample Audio sample to set playhead to */
    void resetPlayhead(int sample) { playhead = sample; }
    
    /** Resets the track state, with optional parameters to specify.
     
     @param[in] initBpm Initial tempo of track
     @param[in] initGain Initial volume gain
     @param[in] initHighPass Initial high-pass frequency */
    void reset(double initBpm = 0.0, double initGain = 0.0, double initHighPass = HIGH_PASS_MAX);
    
    /** Fetches information on the current/next mix transition.
     
     @return Pointer to the current mix information */
    MixInfo* getCurrentMix() { return currentMix; }
    
    
    TrackInfo* info = nullptr; ///< Pointer to information about the currently loaded track
    
    bool leader = false; ///< Indicates whether this track is currently leading the mix
    
    juce::AudioBuffer<float>* audio; ///< Pointer to audio data for the loaded track
    
    InterpolatedParameter bpm; ///< Tempo parameter that can interpolate between its current and target values
    InterpolatedParameter gain; ///< Gain parameter that can interpolate between its current and target values
    InterpolatedParameter highPassFreq; ///< High-pass frequency parameter that can interpolate between its current and target values
    
private:
    
    MixInfo* currentMix; ///< Pointer to information on the current/next mix transition
    
    int playhead = 0; ///< Track playhead position, in audio samples
    
};

#endif /* Track_hpp */
