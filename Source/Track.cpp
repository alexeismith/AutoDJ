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
    currentMix = mix;
    
    if (leader)
    {
        reset(currentMix->bpm);
        
        audio = currentMix->nextTrackAudio;
        if (audio == nullptr)
            return false;
        
        info = currentMix->nextTrack;
        
        playhead = currentMix->followerStart;
        
        gain.moveTo(1.0, currentMix->followerStart, currentMix->followerEnd - currentMix->followerStart);
        highPassFreq.moveTo(0, currentMix->followerStart, currentMix->followerEnd - currentMix->followerStart);
    }
    else
    {
        leader = true;
        
        gain.currentValue = 1.0;
        highPassFreq.currentValue = 0.0;
        
        bpm.moveTo(currentMix->bpm, playhead, currentMix->leaderStart - playhead);
        
        gain.moveTo(0, currentMix->leaderStart, currentMix->leaderEnd - currentMix->leaderStart);
        highPassFreq.moveTo(HIGH_PASS_MAX, currentMix->leaderStart, currentMix->leaderEnd - currentMix->leaderStart);
    }
    
    return true;
}
