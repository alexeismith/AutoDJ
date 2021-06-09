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


/** Stores the test result for the analysis of a single track */
typedef struct AnalysisTestResult {
    bool bpmCorrect; ///< Whether estimated BPM is equal to ground truth
    int bpmError; ///< Absolute difference in BPM between estimate and ground truth
    // Phase: irrelevant if BPM is incorrect...
    double phaseError = -1.0; ///< Phase error as a proportion of the beat period
    bool phaseWithinJnd1 = false; ///< Phase error is within PHASE_JND
    bool phaseWithinJnd2 = false; ///< Phase error is within 2xPHASE_JND
    bool phaseWithinJnd2OrOffbeat = false; ///< Phase error is within 2xPHASE_JND, or on the offbeat
    // Downbeat: irrelevant if phaseWithinJnd2 is incorrect...
    bool downbeatCorrect = false; ///< Whether the estimated downbeat is equal to ground truth
} AnalysisTestResult;


/**
 Extension of AnalysisManager used for testing different anaysis algorithms.
 Instead of writing the analysis results to the track database, this compares them with a database of ground truth data.
 It then performs post-processing on the entire set of results, to produce an overall summary.
 */
class AnalysisTest : public AnalysisManager
{
public:
    
    /** Constructor. */
    AnalysisTest() {}
    
    /** Destructor. */
    ~AnalysisTest() {}
    
    /** Starts the analysis process, launching a number of AnalysisThreads. */
    void startAnalysis(DataManager* dataManager) override;
    
    /** Fetches a progress update for the overall analysis process.
    
    @param[out] progress Fractional progress of analysis (0.0 to 1.0)
    
    @return True if analysis is fully complete (progress variable is not a safe indicator of this) */
    bool isFinished(double& progress) override;
    
    /** Instead of storing the analysis result in the track database, this just calls processResult().
    
     @param[in] track Pointer to the track data */
    void storeAnalysis(TrackInfo* track) override;
    
    /** Checks a single track analysis result against the ground truth data.
    
     @param[in] track Pointer to the track data */
    void processResult(TrackInfo* track) override;
    
private:
    
    /** Outputs the overall test results to the debug console. */
    void printResults();
    
    std::atomic<bool> initialised = false; ///< Indicates whether this has been initialised, ready for anaylsis to start
    
    juce::Array<TrackInfo> groundTruth; ///< Array of ground truth track data, which is copied from the AutoDJ database file before analysis starts
    
    juce::Array<AnalysisTestResult> testResults; ///< Array of test results, comparing the analysis data to the ground truth
    
    int numTracks; ///< Number of tracks analysed, used for averaging the overall results
    
};

#endif /* AnalysisTest_hpp */
