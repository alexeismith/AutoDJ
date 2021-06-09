//
//  AnalysisManager.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 26/03/2021.
//

#ifndef AnalysisManager_hpp
#define AnalysisManager_hpp

#include <JuceHeader.h>
#include "AnalysisThread.hpp"
#include "CommonDefs.hpp"

class DataManager;


/** Overall results of music library analysis. */
typedef struct AnalysisResults
{
    int minBpm = -1; ///< Minimum tempo found
    int maxBpm = -1; ///< Maximum tempo found
    float minGroove; ///< Minimum groove found
    float maxGroove; ///< Maximum groove found
} AnalysisResults;


/**
 Handles the MIR audio analysis process, delegating the actual DSP to a number of AnalysisThreads.
 */
class AnalysisManager
{
public:
    
    /** Constructor. */
    AnalysisManager();
    
    /** Destructor. */
    virtual ~AnalysisManager();
    
    /** Adds a new job to the analysis queue.
     
     @param[in] track Pointer to the track to be analysed */
    void addJob(TrackInfo* track) { jobs.add(track); }
    
    /** Starts the analysis process, launching a number of AnalysisThreads. */
    virtual void startAnalysis(DataManager* dataManager);
    
    /** Pauses/resumes analysis.
     Note that this is not instant: the AnalysisThreads are notified using a thread-safe flag, which they check periodically.*/
    void playPause();
    
    /** Fetches a progress update for the overall analysis process.
     
     @param[out] progress Fractional progress of analysis (0.0 to 1.0)
     
     @return True if analysis is fully complete (progress variable is not a safe indicator of this) */
    virtual bool isFinished(double& progress);
    
    /** Fetches the next analysis job from the queue.
     
     @return Pointer to next track to be analysed */
    TrackInfo* getNextJob();
    
    /** Stores newly analysed track data.
     
     @param[in] track Pointer to the track data */
    virtual void storeAnalysis(TrackInfo* track);
    
    /** Updates the AnalysisResults struct against newly analysed track data.
    
    @param[in] track Pointer to the track data */
    virtual void processResult(TrackInfo* track);
    
    /** Fetches the overall analysis results, which give the range of tempo and groove that was found.
     
     @return Analysis results struct */
    AnalysisResults getResults();
    
    /** Clears the analysis queue. */
    void clearJobs() { jobs.clear(); }
    
protected:
    
    DataManager* dataManager = nullptr; ///< Pointer to the app's track data manager
    
    juce::Array<TrackInfo*> jobs; ///< Queue of tracks to be analysed
    
    juce::CriticalSection lock; ///< RAII lock to ensure thread-safety while acessing data within this class
    
    int jobProgress = 0; // Keeps track of how many jobs have been completed
    int nextJob = 0; // Because of multi-threading we also need to keep track of what job is next (not simply jobProgress+1)
    
private:
    
    juce::OwnedArray<AnalysisThread> threads; ///<  Analysis threads which perform the actual audio processing
    
    AnalysisResults results; ///< Overall analysis results, which give the range of tempo and groove that was found.
    
    bool paused = false; ///< Tracks whether analysis is active or paused
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalysisManager) ///< JUCE macro to add a memory leak detector
};


#endif /* AnalysisManager_hpp */
