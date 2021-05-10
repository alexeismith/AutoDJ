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
    bool update(int numSamples);
    
    // Returns false if there is no next track
    bool applyNextMix(MixInfo* mix);
    
    int getPlayhead() { return playhead; }
    
    void resetPlayhead(int sample) { playhead = sample; }
    
    void reset(double initBpm = 0.0, double initGain = 0.0);
    
    void setCurrentMix(MixInfo* mix) { currentMix = mix; }
    
    MixInfo* getCurrentMix() { return currentMix; }
    
    TrackInfo* info = nullptr;
    
    bool leader = false;
    
    juce::AudioBuffer<float>* audio;
    
    InterpolatedParameter bpm;
    InterpolatedParameter gain;
    
private:
    
    MixInfo* currentMix;
    
    int playhead = 0;
    
};

#endif /* Track_hpp */
