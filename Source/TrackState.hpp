//
//  TrackState.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 21/04/2021.
//

#ifndef TrackState_hpp
#define TrackState_hpp

#include "InterpolatedParameter.hpp"

#include "MixData.hpp"

class ArtificialDJ;

class TrackState
{
public:
    
    TrackState(TrackData track, ArtificialDJ* DJ, MixData* currentMix, double initBpm, double initGain, double initPitch);
    
    ~TrackState() {}
    
    // Returns true if track should be stopped/ejected
    bool update(int playhead);
    
    bool applyNextMix();
    
    TrackData track; // TODO: can get this from currentMix based on leader
    
    ArtificialDJ* dj = nullptr;
    MixData* currentMix = nullptr;
    
    bool leader = false;
    
    int currentSample = 0;
    
    InterpolatedParameter bpm;
    InterpolatedParameter gain;
    InterpolatedParameter pitch;
    
    void reset(double initBpm = 0.0, double initGain = 0.0, double initPitch = 0.0);

};

#endif /* TrackState_hpp */
