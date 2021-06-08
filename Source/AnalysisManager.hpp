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



typedef struct AnalysisResults
{
    int minBpm = -1;
    int maxBpm = -1;
    float minGroove;
    float maxGroove;
} AnalysisResults;

class DataManager;

class AnalysisManager
{
public:
    
    /** Constructor. */
    AnalysisManager();
    
    /** Destructor. */
    virtual ~AnalysisManager();
    
    void addJob(TrackInfo* track) { jobs.add(track); }
    
    virtual void startAnalysis(DataManager* dataManager);
    
    void playPause();
    
    virtual bool isFinished(double& progress);
    
    TrackInfo* getNextJob();
    
    virtual void storeAnalysis(TrackInfo* track);
    
    virtual void processResult(TrackInfo* track);
    
    AnalysisResults getResults();
    
    void clearJobs() { jobs.clear(); }
    
protected:
    
    DataManager* dataManager = nullptr; ///< Pointer to the app's track data manager
    
    juce::Array<TrackInfo*> jobs;
    
    juce::CriticalSection lock; ///< RAII lock to ensure thread-safety while acessing data within this class
    
    int jobProgress = 0; // Keeps track of how many jobs have been completed
    int nextJob = 0; // Because of multi-threading we also need to keep track of what job is next (not simply jobProgress+1)
    
private:
    
    juce::OwnedArray<AnalysisThread> threads;
    
    AnalysisResults results;
    
    bool paused = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalysisManager) ///< JUCE macro to add a memory leak detector
};


#endif /* AnalysisManager_hpp */
