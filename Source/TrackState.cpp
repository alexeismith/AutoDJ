//
//  TrackState.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 21/04/2021.
//

#include "TrackState.hpp"

#include "ArtificialDJ.hpp"


TrackState::TrackState(TrackData t, ArtificialDJ* DJ, MixData* mix, double initBpm, double initGain, double initPitch) :
    track(t), dj(DJ), currentMix(mix)
{
    bpm.moveTo(initBpm);
    gain.moveTo(initGain);
    pitch.moveTo(initPitch);
}


bool TrackState::update(int playhead)
{
    int numSamples = playhead - currentSample;
    
    currentSample = playhead;
    
    bpm.update(playhead, numSamples);
    pitch.update(playhead, numSamples);
    gain.update(playhead, numSamples);
    
    if (!currentMix || (leader && (currentSample >= currentMix->end)))
        return applyNextMix();
    
    return false;
}


void TrackState::reset(double initBpm, double initGain, double initPitch)
{
    leader = false;
    currentSample = 0;
    
    bpm.moveTo(currentMix->bpm);
    gain.moveTo(0.0);
    pitch.moveTo(0.0);
}


bool TrackState::applyNextMix()
{
    currentMix = dj->getNextMix(currentMix);
    
    if (leader)
    {
        reset(currentMix->bpm);
        
        track = currentMix->nextTrack;
        
        gain.moveTo(1.0, currentMix->startNext, currentMix->endNext - currentMix->startNext);
        
        return true;
    }
    else
    {
        leader = true;
        
        bpm.moveTo(currentMix->bpm, currentSample, currentMix->start - currentSample);
        
        gain.moveTo(0, currentMix->start, currentMix->end - currentMix->start);
        
        return false;
    }
}
