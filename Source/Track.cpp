//
//  Track.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 21/04/2021.
//

#include "Track.hpp"

#include "ArtificialDJ.hpp"


bool Track::update()
{
    int numSamples = playhead - lastUpdate;
    lastUpdate = playhead;
    
    bpm.update(playhead, numSamples);
    pitch.update(playhead, numSamples);
    gain.update(playhead, numSamples);
    
    if (!currentMix || (leader && (playhead >= currentMix->end)))
        return true;
    
    return false;
}


void Track::reset(double initBpm, double initGain, double initPitch)
{
    leader = false;
    playhead = 0;
    
    bpm.moveTo(currentMix->bpm);
    gain.moveTo(0.0);
    pitch.moveTo(0.0);
}


bool Track::applyNextMix(MixInfo* mix)
{
    currentMix = mix;
    
    if (leader)
    {
        reset(currentMix->bpm);
        
        info = currentMix->nextTrack;
        audio = currentMix->nextTrackAudio;
        
        gain.moveTo(1.0, currentMix->startNext, currentMix->endNext - currentMix->startNext);
        
        return true;
    }
    else
    {
        leader = true;
        
        bpm.moveTo(currentMix->bpm, playhead, currentMix->start - playhead);
        
        gain.moveTo(0, currentMix->start, currentMix->end - currentMix->start);
        
        return false;
    }
}
