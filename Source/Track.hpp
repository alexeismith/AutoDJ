//
//  Track.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 21/04/2021.
//

#ifndef Track_hpp
#define Track_hpp

#include "InterpolatedParameter.hpp"

#include "MixInfo.hpp"

class Track
{
public:
    
    Track() {}
    
    ~Track() {}
    
    // Returns true if current mix has finished
    bool update();
    
    bool applyNextMix(MixInfo* mix);
    
    TrackInfo info; // TODO: can get this from currentMix based on leader
    juce::AudioBuffer<float>* audio;
    
    MixInfo* currentMix;
    
    bool leader = false;
    
    int playhead = 0;
    int lastUpdate = 0;
    
    InterpolatedParameter bpm;
    InterpolatedParameter gain;
    InterpolatedParameter pitch;
    
    void reset(double initBpm = 0.0, double initGain = 0.0, double initPitch = 0.0);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Track)
};

#endif /* Track_hpp */
