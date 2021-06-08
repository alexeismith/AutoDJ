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

class AnalysisThread : public juce::Thread
{
public:
    
    /** Constructor. */
    AnalysisThread(int ID, AnalysisManager* am, DataManager* dm, essentia::standard::AlgorithmFactory& factory);
    
    /** Destructor. */
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
    
    AnalysisManager* analysisManager = nullptr; ///< Pointer to 
    DataManager* dataManager = nullptr; ///< Pointer to app's track data manager
    
    std::unique_ptr<AnalyserBeats> analyserBeats;
    std::unique_ptr<AnalyserBeatsEssentia> analyserBeatsEssentia;
    std::unique_ptr<AnalyserKey> analyserKey;
    std::unique_ptr<AnalyserGroove> analyserGroove;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalysisThread) ///< JUCE macro to add a memory leak detector
};

#endif /* AnalysisThread_hpp */
