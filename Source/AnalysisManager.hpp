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
#include "AnalyserBeats.hpp"
#include "AnalyserKey.hpp"


class AnalyserThread;

class AnalysisManager
{
public:
    
    AnalysisManager(TrackDataManager* dm) :
        dataManager(dm) {}
    
    ~AnalysisManager() {}
    
    void startAnalysis();
    
    bool isFinished(double& progress);
    
    TrackData getNextJob(bool& finished);
    
private:
    
    bool areThreadsFinished();
    
    juce::OwnedArray<AnalyserThread> threads;
    
    juce::Array<TrackData> jobs;
    
    TrackDataManager* dataManager = nullptr;
    
    std::atomic<int> jobProgress = 0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalysisManager)
};



class AnalyserThread : public juce::Thread
{
public:
    
    AnalyserThread(AnalysisManager* am, TrackDataManager* dm);
    
    ~AnalyserThread() {}
    
    void run();
    
private:
    
    void analyse(TrackData& track);
    
    AnalysisManager* analysisManager = nullptr;
    TrackDataManager* dataManager = nullptr;
    
    std::unique_ptr<AnalyserBeats> analyserBeats;
    std::unique_ptr<AnalyserKey> analyserKey;
    
};

#endif /* AnalysisManager_hpp */
