//
//  AnalysisManager.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 26/03/2021.
//

#ifndef AnalysisManager_hpp
#define AnalysisManager_hpp

#include <JuceHeader.h>

#include "TrackDataManager.hpp"
#include "AnalysisThread.hpp"
#include "CommonDefs.hpp"

class AnalysisManager
{
public:
    
    AnalysisManager(TrackDataManager* dm) :
        dataManager(dm) {}
    
    ~AnalysisManager();
    
    void startAnalysis();
    
    bool isFinished(double& progress);
    
    TrackInfo getNextJob(bool& finished);
    
    void incrementNumAnalysed();
    
    bool minimumAnalysed() { return numAnalysed >= NUM_TRACKS_MIN; }
    
private:
    
    bool areThreadsFinished();
    
    juce::CriticalSection lock;
    
    juce::OwnedArray<AnalysisThread> threads;
    
    juce::Array<TrackInfo> jobs;
    
    TrackDataManager* dataManager = nullptr;
    
    int jobProgress = 0;
    
    int numAnalysed = 0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalysisManager)
};


#endif /* AnalysisManager_hpp */
