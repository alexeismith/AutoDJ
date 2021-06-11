//
//  Track.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 21/04/2021.
//

#include "Track.hpp"

#include "ArtificialDJ.hpp"


bool Track::update(int numSamples)
{
    playhead += numSamples;
    
    bpm.update(playhead, numSamples);
    gain.update(playhead, numSamples);
    highPassFreq.update(playhead, numSamples);
    
    if (leader && playhead >= currentMix->leaderEnd)
        return true;
    
    return false;
}


void Track::reset(double initBpm, double initGain, double initHighPass)
{
    leader = false;
    playhead = 0;
    
    bpm.resetTo(initBpm);
    gain.resetTo(initGain);
    highPassFreq.resetTo(initHighPass);
}


bool Track::applyNextMix(MixInfo* mix)
{
    // Store the provided mix
    currentMix = mix;
    
    // If this track was leading the last transition, it will not be leading this one
    // (i.e. this is the NEW track being mixed in)
    if (leader)
    {
        // Since this track is not playing, we can set it straight to the transition bpm
        reset(currentMix->bpm);
        
        // Fetch the track audio
        audio = currentMix->nextTrackAudio;
        // If there is no audio loaded, there is no track to transition to (happens at end of mix)
        if (audio == nullptr)
            return false;
        
        // Fetch the track info
        info = currentMix->nextTrack;
        
        // Set the playhead to the start position
        playhead = currentMix->followerStart;
        
        // Set up the crossfade modulation, which will occur over the course of the transition
        gain.moveTo(1.0, currentMix->followerStart, currentMix->followerEnd - currentMix->followerStart);
        highPassFreq.moveTo(0, currentMix->followerStart, currentMix->followerEnd - currentMix->followerStart);
    }
    else // Otherwise, this track is now leading the mix
    {
        leader = true;
        
        // Ensure the modulated crossfade parameters have reach their idle state
        gain.currentValue = 1.0;
        highPassFreq.currentValue = 0.0;
        
        // Start a BPM modulation to the transition tempo
        // This will complete when the transition starts
        bpm.moveTo(currentMix->bpm, playhead, currentMix->leaderStart - playhead);
        
        // Set up the crossfade modulation, which will occur over the course of the transition
        gain.moveTo(0, currentMix->leaderStart, currentMix->leaderEnd - currentMix->leaderStart);
        highPassFreq.moveTo(HIGH_PASS_MAX, currentMix->leaderStart, currentMix->leaderEnd - currentMix->leaderStart);
    }
    
    return true;
}
