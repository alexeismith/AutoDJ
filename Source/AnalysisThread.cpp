
//
//  AnalysisThread.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 18/04/2021.
//

#include "AnalysisThread.hpp"

#include "TrackDataManager.hpp"
#include "AnalysisManager.hpp"


AnalysisThread::AnalysisThread(int ID, AnalysisManager* am, TrackDataManager* dm) :
    juce::Thread("AnalysisThread"), id(ID), analysisManager(am), dataManager(dm)
{
    analyserBeats.reset(new AnalyserBeats());
    analyserKey.reset(new AnalyserKey());
    progress.store(0.0);
}


void AnalysisThread::run()
{
    TrackInfo* track = analysisManager->getNextJob();
    
    while (track)
    {
        analyse(*track);
        track = analysisManager->getNextJob();
    }
    
    DBG("Analysis Thread " << id << " Finished");
}


void AnalysisThread::analyse(TrackInfo& track)
{
    juce::AudioBuffer<float>* buffer;
    
    progress.store(0.0);
    
    DBG("Analysis Thread " << id << ": " << track.filename);
    
    buffer = dataManager->loadAudio(track.filename, true);
    
    progress.store(0.1);

    analyserBeats->analyse(buffer, &progress, track.bpm, track.beatPhase, track.downbeat);
    
    if (threadShouldExit()) return;
    
    progress.store(0.8);
    
    analyserKey->analyse(buffer, track.key);
    
    progress.store(0.9);
    
    track.analysed = true;
    
    if (threadShouldExit()) return;
    
    dataManager->storeAnalysis(&track);
    dataManager->releaseAudio(buffer);
    
    progress.store(1.0);
}
