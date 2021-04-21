//
//  TrackState.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 21/04/2021.
//

#include "TrackState.hpp"

#include "ArtificialDJ.hpp"


TrackState::TrackState(ArtificialDJ* DJ, InterpolatedParameter b, InterpolatedParameter p, InterpolatedParameter g) :
    dj(DJ), bpm(b), pitch(p), gain(g)
{
}


bool TrackState::update(int newSample)
{
    int numSamples = newSample - currentSample;
    
    currentSample = newSample;
    
    bpm.update(numSamples);
    pitch.update(numSamples);
    gain.update(numSamples);
    
    return dj->updateTrackState(this);
}
