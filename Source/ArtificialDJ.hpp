//
//  ArtificialDJ.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 20/04/2021.
//

#ifndef ArtificialDJ_hpp
#define ArtificialDJ_hpp

#include <JuceHeader.h>

#include "TrackDataManager.hpp"
#include "MixData.hpp"
#include "TrackState.hpp"

// Need to keep reference to currently executing MixData

// Calculate weighted BPM trajectory once there is already a queue?

class TrackState;

class ArtificialDJ : public juce::Thread
{
public:
    
    ArtificialDJ(TrackDataManager* dm);
    
    ~ArtificialDJ() {}
    
    void run();
    
    // Returns true if track should be stopped/ejected
    bool updateTrackState(TrackState* state);
    
private:
    
    TrackData chooseTrack();
    
    MixData generateMix(TrackData nextTrack);
    
    juce::CriticalSection lock;
    
    juce::Array<MixData> mixQueue;
    
    TrackDataManager* dataManager = nullptr;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ArtificialDJ)
};

#endif /* ArtificialDJ_hpp */
