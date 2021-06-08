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
#include "MixInfo.hpp"
#include "Track.hpp"
#include "TrackChooser.hpp"
#include "AnalyserSegments.hpp"

// Need to keep reference to currently executing MixInfo

// Calculate weighted BPM trajectory once there is already a queue?

class Track;

class ArtificialDJ : public juce::Thread
{
public:
    
    /** Constructor. */
    ArtificialDJ(DataManager* dm);
    
    /** Destructor. */
    ~ArtificialDJ() { stopThread(5000); }
    
    void run();
    
    void setAudioProcessor(AudioProcessor* am) { audioProcessor = am; }
    
    // Returns true if new track should be loaded
    MixInfo getNextMix(MixInfo current);
    
    bool playPause();
    
    bool isInitialised() { return initialised.load(); }
    
    bool isMixReady(int minimumReady = 1);
    
    bool canSkip();
    
    void reset();
    
private:
    
    void removeMix(MixInfo mix);
    
    void initialise();
    
    void generateMix() { generateMixComplex(); }
    
    void generateMixSimple();
    
    void generateMixComplex();
    
    std::unique_ptr<TrackChooser> chooser;
    
    std::atomic<bool> initialised;
    
    bool playing = false;
    std::atomic<bool> ending = false;
    std::atomic<bool> endingConfirm = false;
    
    int mixIdCounter = 0;
    
    juce::CriticalSection lock; ///< RAII lock to ensure thread-safety while acessing data within this class
    
    juce::Array<MixInfo> mixQueue;
    
    DataManager* dataManager = nullptr; ///< Pointer to the app's track data manager
    AudioProcessor* audioProcessor = nullptr; ///< Pointer to the top-level audio processor
    
    RandomGenerator randomGenerator;
    
    AnalyserSegments segmenter;
    
    TrackInfo* leadingTrack = nullptr; ///< Pointer to information of the current track being played
    juce::Array<int> leadingTrackSegments;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ArtificialDJ) ///< JUCE macro to add a memory leak detector
};

#endif /* ArtificialDJ_hpp */
