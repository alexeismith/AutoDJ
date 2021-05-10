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
    
    if (leader && playhead >= currentMix->end)
        return true;
    
    return false;
}


void Track::reset(double initBpm, double initGain)
{
    leader = false;
    playhead = 0;
    
    bpm.resetTo(initBpm);
    gain.resetTo(initGain);
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
        
        playhead = currentMix->startNext;
        
        gain.moveTo(1.0, currentMix->startNext, currentMix->endNext - currentMix->startNext);
    }
    else
    {
        leader = true;
        
        gain.currentValue = 1.0;
        
        bpm.moveTo(currentMix->bpm, playhead, currentMix->start - playhead);
        
        gain.moveTo(0, currentMix->start, currentMix->end - currentMix->start);
    }
    
    return true;
}
