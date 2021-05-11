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

class TrackDataManager;


class AnalysisManager
{
public:
    
    AnalysisManager();
    
    virtual ~AnalysisManager();
    
    void addJob(TrackInfo* track) { jobs.add(track); }
    
    virtual void startAnalysis(TrackDataManager* dataManager);
    
    void playPause();
    
    virtual bool isFinished(double& progress);
    
    TrackInfo* getNextJob();
    
    void storeAnalysis(TrackInfo* track);
    
    virtual void processResult(TrackInfo* track);
    
    AnalysisResults getResults();
    
    void clearJobs() { jobs.clear(); }
    
protected:
    
    TrackDataManager* dataManager;
    
    juce::Array<TrackInfo*> jobs;
    
private:
    
    juce::CriticalSection lock;
    
    juce::OwnedArray<AnalysisThread> threads;
    
    AnalysisResults results;
    
    int jobProgress = 0; // Keeps track of how many jobs have been completed
    int nextJob = 0; // Because of multi-threading we also need to keep track of what job is next (not simply jobProgress+1)
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalysisManager)
};


#endif /* AnalysisManager_hpp */
