//
//  AnalysisThread.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 18/04/2021.
//

#ifndef AnalysisThread_hpp
#define AnalysisThread_hpp

#include <JuceHeader.h>
#include "TrackInfo.hpp"
#include "AnalyserBeats.hpp"
#include "AnalyserBeatsEssentia.hpp"
#include "AnalyserKey.hpp"
#include "AnalyserGroove.hpp"

class DataManager;
class AnalysisManager;


/**
 Manages the analysis of individual tracks, delegating the MIR work to a number of specialised classes.
 */
class AnalysisThread : public juce::Thread
{
public:
    
    /** Constructor. */
    AnalysisThread(int ID, AnalysisManager* am, DataManager* dm, essentia::standard::AlgorithmFactory& factory);
    
    /** Destructor. */
    ~AnalysisThread() {}
    
    /** Thread running loop, which will continue until there are no more tracks to analyse. */
    void run();
    
    /** Fetches analysis progress for the current track.
     
     @return Analysis progress (0.0 to 1.0) */
    double getProgress() { return progress.load(); }
    
    /** Pauses/resumes analysis processing.
     Note that this is not instant: the thread-safe 'pause' flag is checked periodically on the analysis thread.*/
    void playPause() { pause.store(!pause.load()); }
    
private:
    
    /** Pushes the provided track through the analysis pipeline, first calling DataManager to load the associated audio data.
     
     @param[in,out] track Track to be analysed - note the results are stored in this reference variable */
    void analyse(TrackInfo& track);
    
    /** Called periodically during analysis to check if the thread should sleep or exit (or neither).
     
     @return True if the thread should exit, in which case the current analysis will be aborted */
    bool checkPauseOrExit();
    
    int id; ///< Unique identifier for this thread
    
    std::atomic<double> progress = 0.0; ///< Progress of the current track analysis
    
    std::atomic<bool> pause = false; ///< Tracks whether analysis is active or paused
    
    AnalysisManager* analysisManager = nullptr; ///< Pointer to the manager of this thread
    DataManager* dataManager = nullptr; ///< Pointer to the app's track data manager
    
    // Analysis handlers
    std::unique_ptr<AnalyserBeats> analyserBeats; ///< Temporal MIR analyser (using QM-DSP algorithms)
    std::unique_ptr<AnalyserBeatsEssentia> analyserBeatsEssentia; ///< Temporal MIR analyser (using QM-DSP and Essentia algorithms)
    std::unique_ptr<AnalyserKey> analyserKey; ///< Tonal MIR analyser
    std::unique_ptr<AnalyserGroove> analyserGroove; ///< Danceability analyser
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalysisThread) ///< JUCE macro to add a memory leak detector
};

#endif /* AnalysisThread_hpp */
