//
//  MixInfo.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 21/04/2021.
//

#ifndef MixInfo_hpp
#define MixInfo_hpp

#include "TrackInfo.hpp"

#include <JuceHeader.h>


typedef struct MixInfo {
    int id = -1;
    TrackInfo* leadingTrack; // Existing trexack that is playing
    TrackInfo* nextTrack; // New track to be mixed in
    juce::AudioBuffer<float>* nextTrackAudio = nullptr;
    int leaderStart = 0; // Sample position in leading track where mix begins
    int leaderEnd = 0; // Sample position in leading track where mix finishes
    int followerStart = 0; // Sample position in next track where mix begins
    int followerEnd = 0; // Sample position in next track where mix finishes
    double bpm = 0.0; // Tempo that new and previous tracks must meet by the start of the mix
} MixInfo;


#endif /* MixInfo_hpp */
