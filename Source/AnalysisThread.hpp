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

class TrackDataManager;
class AnalysisManager;

class AnalysisThread : public juce::Thread
{
public:
    
    AnalysisThread(int ID, AnalysisManager* am, TrackDataManager* dm, essentia::standard::AlgorithmFactory& factory);
    
    ~AnalysisThread() {}
    
    void run();
    
    double getProgress() { return progress.load(); }
    
    void playPause() { pause.store(!pause.load()); }
    
private:
    
    void analyse(TrackInfo& track);
    
    bool shouldExit();
    
    int id;
    
    std::atomic<double> progress = 0.0;
    
    std::atomic<bool> pause = false;
    
    AnalysisManager* analysisManager = nullptr;
    TrackDataManager* dataManager = nullptr;
    
    std::unique_ptr<AnalyserBeats> analyserBeats;
    std::unique_ptr<AnalyserBeatsEssentia> analyserBeatsEssentia;
    std::unique_ptr<AnalyserKey> analyserKey;
    std::unique_ptr<AnalyserGroove> analyserGroove;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalysisThread)
};

#endif /* AnalysisThread_hpp */
