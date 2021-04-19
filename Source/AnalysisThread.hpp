//
//  AnalysisThread.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 18/04/2021.
//

#ifndef AnalysisThread_hpp
#define AnalysisThread_hpp

#include <JuceHeader.h>

#include "TrackDataManager.hpp"
#include "AnalyserBeats.hpp"
#include "AnalyserKey.hpp"

class AnalysisManager;

class AnalysisThread : public juce::Thread
{
public:
    
    AnalysisThread(int ID, AnalysisManager* am, TrackDataManager* dm);
    
    ~AnalysisThread() { stopThread(1000); }
    
    void run();
    
    double getProgress() { return progress.load(); }
    
private:
    
    void analyse(TrackData& track);
    
    int id;
    
    std::atomic<double> progress;
    
    AnalysisManager* analysisManager = nullptr;
    TrackDataManager* dataManager = nullptr;
    
    std::unique_ptr<AnalyserBeats> analyserBeats;
    std::unique_ptr<AnalyserKey> analyserKey;
    
};

#endif /* AnalysisThread_hpp */
