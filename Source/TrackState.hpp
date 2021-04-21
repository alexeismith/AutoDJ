//
//  TrackState.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 21/04/2021.
//

#ifndef TrackState_hpp
#define TrackState_hpp

#include "InterpolatedParameter.hpp"

class ArtificialDJ;

class TrackState
{
public:
    
    TrackState(ArtificialDJ* DJ, InterpolatedParameter bpm, InterpolatedParameter pitch, InterpolatedParameter gain);
    
    ~TrackState() {}
    
    // Returns true if track should be stopped/ejected
    bool update(int newSample);
    
    ArtificialDJ* dj = nullptr;
    
    bool leader = false;
    
    int currentSample = 0;
    
    InterpolatedParameter bpm;
    InterpolatedParameter pitch;
    InterpolatedParameter gain;

};

#endif /* TrackState_hpp */
