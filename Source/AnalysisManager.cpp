//
//  AnalysisManager.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 26/03/2021.
//

#include "AnalysisManager.hpp"

#define MAX_NUM_THREADS (8)


AnalysisManager::~AnalysisManager()
{
    // Set progress tracker to completion, so that no more jobs are given to analysis threads
    jobProgress = jobs.size();
    
    for (auto* thread : threads)
    {
        thread->stopThread(10000);
    }
}


void AnalysisManager::startAnalysis()
{
    juce::Array<TrackData>* tracks = dataManager->getTracks();
    
    int numThreads = juce::SystemStats::getNumCpus();
    DBG("Num logical CPU cores: " << numThreads);
    numThreads -= 2;
    numThreads = juce::jmin(numThreads, MAX_NUM_THREADS);
    DBG("Using " << numThreads << " analysis threads");
    
    for (int i = 0; i < tracks->size(); i++)
    {
        if (tracks->getReference(i).analysed == false)
            jobs.add(tracks->getReference(i));
    }
    
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
    
    for (int i = 0; i < numThreads; i++)
    {
        threads.add(new AnalysisThread(i+1, this, dataManager));
        threads.getUnchecked(i)->startThread();
    }
}


bool AnalysisManager::isFinished(double& progress)
{
    const juce::ScopedLock sl (lock);
    
    progress = double(jobProgress) / jobs.size();
    
    if (jobProgress >= jobs.size())
    {
        if (areThreadsFinished())
            return true;
    }
    
    for (auto* thread : threads)
    {
        progress -= (1.0 - thread->getProgress()) / jobs.size();
    }
    
    return false;
}


bool AnalysisManager::areThreadsFinished()
{
    for (int i = 0; i < threads.size(); i++)
    {
        if (threads.getUnchecked(i)->isThreadRunning())
            return false;
    }
    
    return true;
}


TrackData AnalysisManager::getNextJob(bool& finished)
{
    const juce::ScopedLock sl (lock);
    
    int job = jobProgress;
    
    if (job == jobs.size())
    {
        finished = true;
        return TrackData();
    }
    else
    {
        finished = false;
        jobProgress += 1;
        return jobs.getUnchecked(job);
    }
}
