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

class Track;


/**
 The artificial DJ brain which makes mixing decisions.
 These decisions are made in advance and put into a queue (an array of MixInfo).
 */
class ArtificialDJ : public juce::Thread
{
public:
    
    /** Constructor. */
    ArtificialDJ(DataManager* dm);
    
    /** Destructor. */
    ~ArtificialDJ() { stopThread(5000); }
    
    /** Starts the DJ thread, which makes decisions in a loop until the app exits. */
    void run();
    
    /** Sets the top-level audio processor.
     This is done after construction because the audio processor requires a reference to this as well.
     
     @param[in] processor Pointer to the top-level audio processor instance */
    void setAudioProcessor(AudioProcessor* processor) { audioProcessor = processor; }
    
    /** Fetches the data associated with the next mix/transition.
     
     @return Struct containing all the data necessary to execute the next transition */
    MixInfo getNextMix(MixInfo current);
    
    /** Toggles the playback state of the DJ mix.
     
     @return False if the DJ needs to generate mixes before playback begins */
    bool playPause();
    
    /** Checks whether the DJ is initialised.
     The DJ is initialised if the first two tracks are chosen and the transition between them is generated.
     
     @return State of DJ initialisation */
    bool isInitialised() { return initialised.load(); }
    
    /** Checks whether the mixing decision queue is populated.
     
     @param[in] minimumReady Minimum number of transitions required to return true
     
     @return Result of the check */
    bool isMixReady(int minimumReady = 1);
    
    /** Checks whether the DJ is ready to skip to the next mix event.
     This requires at least 2 transitions ready in the queue.
     
     @return Result of the check */
    bool canSkip();
    
    /** Resets the state of the DJ, ready for a new performance. */
    void reset();
    
private:
    
    /** Removes a mix from the queue, if it is present.
     This checks against the ID of the supplied mix.
     
     @param[in] mix Mix information to be removed */
    void removeMix(MixInfo mix);
    
    /** Initialises the DJ mix, choosing the first two tracks and
     the transition to be made between them.*/
    void initialise();
    
    /** Wrapper for generating a mix transition.
     Choose either generateMixSimple() or generateMixComplex() here. */
    void generateMix() { generateMixComplex(); }
    
    /** Generates a transition between two tracks, using simple fixed parameters. */
    void generateMixSimple();
    
    /** Generates a transition between two tracks, taking into account their content, as well as randomness. */
    void generateMixComplex();
    
    
    juce::CriticalSection lock; ///< RAII lock to ensure thread-safety while acessing data within this class
    
    std::atomic<bool> initialised; ///< Flag to indicate whether the DJ has been initialised
    
    DataManager* dataManager = nullptr; ///< Pointer to the app's track data manager
    AudioProcessor* audioProcessor = nullptr; ///< Pointer to the top-level audio processor
    
    std::unique_ptr<TrackChooser> chooser; ///< Handles the choice of tracks to play
    
    RandomGenerator randomGenerator; ///< Generates random values to influence mixing decisions
    
    AnalyserSegments segmenter; ///< Finds musical sections within the tracks
    
    juce::Array<MixInfo> mixQueue; ///< Queue of mixing decisions, grouped as transitions between tracks
    
    TrackInfo* leadingTrack = nullptr; ///< Pointer to information of the current track being played
    juce::Array<int> leadingTrackSegments; ///< Array of boundary points between musical section detected in the leading track
    
    bool playing = false; ///< Flag to track mix playback state
    std::atomic<bool> ending = false; ///< Indicates that the mix will end if there are no more tracks added
    std::atomic<bool> endingConfirm = false; ///< Indicates that the mix IS ending
    
    int mixIdCounter = 0; ///< Keeps track of the ID for the next mix to be generated
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ArtificialDJ) ///< JUCE macro to add a memory leak detector
};

#endif /* ArtificialDJ_hpp */
