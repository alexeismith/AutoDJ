//
//  AnalysisManager.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 26/03/2021.
//

#include "AnalysisManager.hpp"

#include "TrackDataManager.hpp"

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


void AnalysisManager::startAnalysis(TrackDataManager* dataManager)
{
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


bool AnalysisManager::isFinished(double& progress)
{
    const juce::ScopedLock sl(lock);
    
    if (jobProgress >= jobs.size())
        return true;
    
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


void AnalysisManager::incrementProgress()
{
    const juce::ScopedLock sl(lock);
    
    jobProgress += 1;
}
