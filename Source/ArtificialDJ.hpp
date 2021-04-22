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
#include "AudioProcessor.hpp"
#include "MixData.hpp"
#include "TrackState.hpp"

// Need to keep reference to currently executing MixData

// Calculate weighted BPM trajectory once there is already a queue?

class TrackState;

class ArtificialDJ : public juce::Thread
{
public:
    
    ArtificialDJ(TrackDataManager* dm);
    
    ~ArtificialDJ() { stopThread(1000); }
    
    void run();
    
    void setAudioProcessor(AudioProcessor* am) { audioProcessor = am; }
    
    // Returns true if new track should be loaded
    bool checkMixState(TrackState* state);
    
    // Returns true if new track should be loaded
    MixData* getNextMix(MixData* current);
    
    void playPause();
    
private:
    
    void initialise();
    
    std::atomic<bool> initialised;
    bool playing = false;
    
    TrackData* chooseTrack(bool random);
    
    void generateMix(TrackData leadingTrack, TrackData nextTrack);
    
    juce::CriticalSection lock;
    
    juce::Array<MixData> mixQueue;
    
    TrackDataManager* dataManager = nullptr;
    AudioProcessor* audioProcessor = nullptr;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ArtificialDJ)
};

#endif /* ArtificialDJ_hpp */
