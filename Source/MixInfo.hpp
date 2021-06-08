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


/**
 Holds all the mixing decisions related to a single DJ transition.
 */
typedef struct MixInfo {
    int id = -1; ///< Unique ID of the transition
    TrackInfo* leadingTrack; ///< Pointer to information of track to be mixed out
    TrackInfo* nextTrack; ///< Pointer to information of new track to be mixed in
    juce::AudioBuffer<float>* nextTrackAudio = nullptr; ///< Pointer to audio data for the track to be mixed in
    int leaderStart = 0; ///< Position / audio sample in leading track where mix begins
    int leaderEnd = 0; ///< Position / audio sample in leading track where mix finishes
    int followerStart = 0; ///< Position / audio sample in next track where mix begins
    int followerEnd = 0; ///< Position / audio sample in next track where mix finishes
    double bpm = 0.0; ///< Tempo that new and previous tracks must meet by the start of the mix
} MixInfo;


#endif /* MixInfo_hpp */
