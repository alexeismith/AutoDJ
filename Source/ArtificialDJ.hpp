//
//  ArtificialDJ.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 20/04/2021.
//

#ifndef ArtificialDJ_hpp
#define ArtificialDJ_hpp

#include <JuceHeader.h>

#include "AudioProcessor.hpp"
#include "TrackDataManager.hpp"
#include "MixData.hpp"

// Need to keep reference to currently executing MixData

// Calculate weighted BPM trajectory once there is already a queue?

class ArtificialDJ : public juce::Thread
{
public:
    
    ArtificialDJ(AudioProcessor* p, TrackDataManager* dm);
    
    ~ArtificialDJ() {}
    
    void run();
    
    void updateMix(MixState& state);
    
private:
    
    TrackData chooseTrack();
    
    MixData generateMix(TrackData nextTrack);
    
    juce::CriticalSection lock;
    
    juce::Array<MixData> mixQueue;
    
    AudioProcessor* audioProcessor = nullptr;
    TrackDataManager* dataManager = nullptr;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ArtificialDJ)
};

#endif /* ArtificialDJ_hpp */
