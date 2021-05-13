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
    double phaseError = -1.0; // Phase error as a proportion of the beat period, irrelevant if BPM is incorrect
    bool phaseWithinJnd1 = false; // Phase error is within PHASE_JND, irrelevant if BPM is incorrect
    bool phaseWithinJnd2 = false; // Phase error is within 2xPHASE_JND, irrelevant if BPM is incorrect
    bool downbeatCorrect = false; // Irrelevant if phaseWithinJnd2 is incorrect
} AnalysisTestResult;


class AnalysisTest : public AnalysisManager
{
public:
    
    AnalysisTest() {}
    
    ~AnalysisTest() {}
    
    void startAnalysis(TrackDataManager* dataManager) override;
    
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
