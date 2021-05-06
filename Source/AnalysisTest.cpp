//
//  AnalysisTest.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 06/05/2021.
//

#include "AnalysisTest.hpp"

#include "TrackDataManager.hpp"


void AnalysisTest::startAnalysis(TrackDataManager* dataManager)
{
    jassert(jobs.isEmpty()); // Invalid ground truth data: all tracks should be marked as analysed
    
    numTracks = dataManager->getNumTracks();
    
    for (int i = 0; i < numTracks; i++)
    {
        groundTruth.add(dataManager->getTracks()[i]);
        jobs.add(&dataManager->getTracks()[i]);
    }
    
    initialised.store(true);

    AnalysisManager::startAnalysis(dataManager);
}


bool AnalysisTest::isFinished(double& progress)
{
    bool finished = AnalysisManager::isFinished(progress);
    
    if (finished)
        printResults();
    
    return finished;
}


void AnalysisTest::processResult(TrackInfo* estimate)
{
    AnalysisTestResult testResult; // Stores the test result for this track
    TrackInfo* truth; // Will point to the ground truth data for this track
    
    // Pass result to superclass function
    AnalysisManager::processResult(estimate);
    
    // This function will get called by TrackDataManager before the test has been initialised
    // Return when this happens
    if (!initialised.load()) return;
    
    // Find the ground truth data for this track
    for (TrackInfo& track : groundTruth)
    {
        if (track.hash == estimate->hash)
            truth = &track;
    }
    
    // Determine whether to estimated bpm is correct
    testResult.bpmCorrect = (truth->bpm == estimate->bpm);
    // Store the absolute BPM error
    testResult.bpmError = abs(truth->bpm - estimate->bpm);
    
    // If the BPM is accurate, process the phase result
    if (testResult.bpmCorrect)
    {
        // Find the absolute error of the estimate (measured in samples)
        int phaseError = abs(truth->beatPhase - estimate->beatPhase);
        // Store this as a proportion of the number of samples per beat
        testResult.phaseError = double(phaseError) / truth->getBeatPeriod();
        // Determine whether the error is within 1x or 2x the Just Noticeable Difference constant
        testResult.phaseWithinJnd1 = (testResult.phaseError <= PHASE_JND);
        testResult.phaseWithinJnd2 = (testResult.phaseError <= 2*PHASE_JND);
    }
    else // Otherwise, the phase is irrelevant
    {
        testResult.phaseWithinJnd1 = false;
        testResult.phaseWithinJnd2 = false;
    }
    
    // If the phase is accurate, process the downbeat result
    if (testResult.phaseWithinJnd2)
    {
        testResult.downbeatCorrect = (truth->downbeat == estimate->downbeat);
    }
    else // Otherwise, the downbeat is irrelevant
    {
        testResult.downbeatCorrect = false;
    }
    
    // Store the results for this track
    testResults.add(testResult);
}


void AnalysisTest::printResults()
{
    // Initialise error sums
    double bpmErrorSum = 0;
    double phaseErrorSum = 0;
    
    // Initialise counters
    int numBpmCorrect = 0;
    int numPhaseWithinJnd1 = 0;
    int numPhaseWithinJnd2 = 0;
    int numDownbeatCorrect = 0;
    
    for (auto result : testResults)
    {
        bpmErrorSum += result.bpmError;
        
        if (result.bpmCorrect)
        {
            numBpmCorrect += 1;
            phaseErrorSum += result.phaseError;
        }
            
        if (result.phaseWithinJnd1)
            numPhaseWithinJnd1 += 1;
        if (result.phaseWithinJnd2)
            numPhaseWithinJnd2 += 1;
        if (result.downbeatCorrect)
            numDownbeatCorrect += 1;
    }
    
    // Calculate evaluation metrics...
    
    double bpmAccuracy = double(numBpmCorrect) / numTracks;
    double averageBpmError = bpmErrorSum / numTracks;
    
    double phaseAccuracyJnd1 = double(numPhaseWithinJnd1) / numBpmCorrect;
    double phaseAccuracyJnd2 = double(numPhaseWithinJnd2) / numBpmCorrect;
    double averagePhaseError = phaseErrorSum / numBpmCorrect;
    
    double downbeatAccuracy = double(numDownbeatCorrect) / numPhaseWithinJnd2;
    
    // Print metrics
    DBG("ANALYSIS TEST RESULTS...");
    DBG("\nBPM Accuracy: " << bpmAccuracy);
    DBG("Average BPM Error: " << averageBpmError);
    DBG("\nPhase Accuracy Within 1xJND: " << phaseAccuracyJnd1);
    DBG("Phase Accuracy Within 2xJND: " << phaseAccuracyJnd2);
    DBG("Average Phase Error: " << averagePhaseError);
    DBG("\nDownbeat Accuracy: " << downbeatAccuracy);
}
