//
//  AnalysisManager.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 26/03/2021.
//

#include "AnalysisManager.hpp"

#include "DataManager.hpp"

#define MAX_NUM_THREADS (8)


AnalysisManager::AnalysisManager()
{
    essentia::init();
}


AnalysisManager::~AnalysisManager()
{
    // Set progress tracker to completion, so that no more jobs are given to analysis threads
    nextJob = jobs.size();
    
    for (auto* thread : threads)
    {
        thread->stopThread(10000);
    }
}


void AnalysisManager::startAnalysis(DataManager* dm)
{
    dataManager = dm;
    
    int numThreads = juce::SystemStats::getNumCpus();
    DBG("Num logical CPU cores: " << numThreads);
    numThreads -= 2;
    numThreads = juce::jmin(numThreads, MAX_NUM_THREADS);
    DBG("Using " << numThreads << " analysis threads");
    
    if (jobs.size() == 0)
    {
        DBG("All tracks already analysed");
        return;
    }
    
    if (jobs.size() < numThreads)
    {
        DBG("Reducing number of threads to match " << jobs.size() << " jobs");
        numThreads = jobs.size();
    }
    
    essentia::standard::AlgorithmFactory& factory = essentia::standard::AlgorithmFactory::instance();
    
    for (int i = 0; i < numThreads; i++)
    {
        threads.add(new AnalysisThread(i+1, this, dataManager, factory));
        threads.getUnchecked(i)->startThread();
    }
}


void AnalysisManager::playPause()
{
    const juce::ScopedLock sl(lock);
    
    for (auto* thread : threads)
    {
        thread->playPause();
    }
    
    paused = !paused;
}


bool AnalysisManager::isFinished(double& progress)
{
    const juce::ScopedLock sl(lock);
    
    if (jobProgress >= jobs.size())
    {
        DBG("Analysis finished. minBpm: " << results.minBpm << " maxBpm: " << results.maxBpm << " minGroove: " << results.minGroove << " maxGroove: " << results.maxGroove);
        return true;
    }
    
    progress = double(jobProgress) / jobs.size();
    
    for (auto* thread : threads)
    {
        progress += thread->getProgress() / jobs.size();
    }
    
    return false;
}


TrackInfo* AnalysisManager::getNextJob()
{
    const juce::ScopedLock sl(lock);
    
    int job = nextJob;
    
    if (job >= jobs.size())
    {
        return nullptr;
    }
    else
    {
        nextJob += 1;
        return jobs.getUnchecked(job);
    }
}


void AnalysisManager::storeAnalysis(TrackInfo* track)
{
    const juce::ScopedLock sl(lock);
    
    processResult(track);
    
    dataManager->storeAnalysis(track);
    
    jobProgress += 1;
}


void AnalysisManager::processResult(TrackInfo* track)
{
    // If this is the first result to be processed,
    // simply copy the bpm and groove values into the results struct and exit
    if (results.minBpm == -1)
    {
        results.minBpm = track->bpm;
        results.maxBpm = track->bpm;
        results.minGroove = track->groove;
        results.maxGroove = track->groove;
        return;
    }
    
    // Compare the current result to the min/max values stored
    // in the results struct and replace them if appropriate
    
    if (track->bpm < results.minBpm)
        results.minBpm = track->bpm;
    else if (track->bpm > results.maxBpm)
        results.maxBpm = track->bpm;
    
    if (track->groove < results.minGroove)
        results.minGroove = track->groove;
    else if (track->groove > results.maxGroove)
        results.maxGroove = track->groove;
}


AnalysisResults AnalysisManager::getResults()
{
    const juce::ScopedLock sl(lock);
    return results;
}
