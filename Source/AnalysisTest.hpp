//
//  AnalysisTest.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 06/05/2021.
//

#ifndef AnalysisTest_hpp
#define AnalysisTest_hpp

#include "AnalysisManager.hpp"

#define PHASE_JND (0.025)

typedef struct AnalysisTestResult {
    bool bpmCorrect; // Whether estimated BPM is equal to ground truth
    int bpmError; // Absolute difference in BPM between estimate and ground truth
    // Phase: irrelevant if BPM is incorrect...
    double phaseError = -1.0; // Phase error as a proportion of the beat period
    bool phaseWithinJnd1 = false; // Phase error is within PHASE_JND
    bool phaseWithinJnd2 = false; // Phase error is within 2xPHASE_JND
    bool phaseWithinJnd2OrOffbeat = false; // Phase error is within 2xPHASE_JND, or on the offbeat
    // Downbeat: irrelevant if phaseWithinJnd2 is incorrect...
    bool downbeatCorrect = false;
} AnalysisTestResult;


class AnalysisTest : public AnalysisManager
{
public:
    
    /** Constructor. */
    AnalysisTest() {}
    
    /** Destructor. */
    ~AnalysisTest() {}
    
    void startAnalysis(DataManager* dataManager) override;
    
    bool isFinished(double& progress) override;
    
    void processResult(TrackInfo* track) override;
    
    void storeAnalysis(TrackInfo* track) override;
    
private:
    
    void printResults();
    
    std::atomic<bool> initialised = false;
    
    juce::Array<TrackInfo> groundTruth;
    
    juce::Array<AnalysisTestResult> testResults;
    
    int numTracks;
    
};

#endif /* AnalysisTest_hpp */
