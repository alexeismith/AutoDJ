
//
//  AnalysisThread.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 18/04/2021.
//

#include "AnalysisThread.hpp"

#include "TrackDataManager.hpp"
#include "AnalysisManager.hpp"


AnalysisThread::AnalysisThread(int ID, AnalysisManager* am, TrackDataManager* dm, essentia::standard::AlgorithmFactory& factory) :
    juce::Thread("AnalysisThread" + juce::String(ID)), id(ID), analysisManager(am), dataManager(dm)
{
    analyserBeats.reset(new AnalyserBeats());
    analyserBeatsEssentia.reset(new AnalyserBeatsEssentia(factory));
    analyserKey.reset(new AnalyserKey());
    analyserGroove.reset(new AnalyserGroove(factory));
    progress.store(0.0);
}


void AnalysisThread::run()
{
    TrackInfo* track = analysisManager->getNextJob();
    
    while (track)
    {
        analyse(*track);
        progress.store(0.0);
        track = analysisManager->getNextJob();
    }
    
    DBG("Analysis Thread " << id << " Finished");
}


void AnalysisThread::analyse(TrackInfo& track)
{
    juce::AudioBuffer<float>* buffer;
    
    DBG("Analysis Thread " << id << ": " << track.getFilename());
    
    buffer = dataManager->loadAudio(track.getFilename(), true);
    
    if (shouldExit()) return;
    
    progress.store(0.1);

    analyserBeatsEssentia->analyse(buffer, &progress, track.bpm, track.beatPhase, track.downbeat);
    
    if (shouldExit()) return;
    
    progress.store(0.7);
    
    analyserKey->analyse(buffer, track.key);
    
    progress.store(0.8);
    
    analyserGroove->analyse(buffer, track.groove);
    
    progress.store(0.9);
    
    if (shouldExit()) return;
    
    track.analysed = true;
    
    dataManager->releaseAudio(buffer);
    
    if (shouldExit()) return;
    
    analysisManager->storeAnalysis(&track);
    
    progress.store(1.0);
}


bool AnalysisThread::shouldExit()
{
    while (pause.load() && !threadShouldExit())
        sleep(1000);
    
    if (threadShouldExit())
        return true;
    
    return false;
}
