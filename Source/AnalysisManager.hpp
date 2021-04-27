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
    
    AnalysisManager() {}
    
    ~AnalysisManager();
    
    void addJob(TrackInfo* track) { jobs.add(track); }
    
    void startAnalysis(TrackDataManager* dataManager);
    
    bool isFinished(double& progress);
    
    TrackInfo* getNextJob();
    
private:
    
    bool areThreadsFinished();
    
    juce::CriticalSection lock;
    
    juce::OwnedArray<AnalysisThread> threads;
    
    juce::Array<TrackInfo*> jobs;
    
    int jobProgress = 0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalysisManager)
};


#endif /* AnalysisManager_hpp */
