//
//  AnalysisManager.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 26/03/2021.
//

#include "AnalysisManager.hpp"

#define MAX_NUM_THREADS (8)


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
    
    for (int i = 0; i < numThreads; i++)
    {
        threads.add(new AnalysisThread(i+1, this, dataManager));
        threads.getUnchecked(i)->startThread();
    }
}


bool AnalysisManager::isFinished(double& progress)
{
    int job = jobProgress.load();
    
    progress = double(job) / jobs.size();
    
    if (job >= jobs.size())
    {
        if (areThreadsFinished())
            return true;
    }
    
//    for (auto* thread : threads)
//    {
//        thread->
//    }
    
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
    int job = jobProgress.load();
    jobProgress.store(job + 1);
    
    if (job >= jobs.size())
    {
        finished = true;
        return TrackData();
    }
    else
    {
        finished = false;
        return jobs.getUnchecked(job);
    }
}
