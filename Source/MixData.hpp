//
//  MixData.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 21/04/2021.
//

#ifndef MixData_hpp
#define MixData_hpp

#include "TrackData.hpp"


typedef struct MixData {
    TrackData leadingTrack; // Existing trexack that is playing
    TrackData nextTrack; // New track to be mixed in
    int start = 0; // Sample position in leading track where mix begins
    int end = 0; // Sample position in leading track where mix finishes
    int startNext = 0; // Sample position in next track where mix begins
    int endNext = 0; // Sample position in next track where mix finishes
    double bpm = 0.0; // Tempo that new and previous tracks must meet by the start of the mix
} MixData;


#endif /* MixData_hpp */
