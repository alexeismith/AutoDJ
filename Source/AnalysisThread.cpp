
//
//  AnalysisThread.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 18/04/2021.
//

#include "AnalysisThread.hpp"

#include "DataManager.hpp"
#include "AnalysisManager.hpp"

#include "BeatTests.hpp"
#include "PerformanceMeasure.hpp"


AnalysisThread::AnalysisThread(int ID, AnalysisManager* am, DataManager* dm, essentia::standard::AlgorithmFactory& factory) :
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
    
    if (checkPauseOrExit()) return;
    
    progress.store(0.1);
    
    PERFORMANCE_START

#ifdef BEATS_QM
    analyserBeats->analyse(buffer, &progress, track.bpm, track.beatPhase, track.downbeat);
#else
    analyserBeatsEssentia->analyse(buffer, &progress, track.bpm, track.beatPhase, track.downbeat);
#endif
    
    PERFORMANCE_END
    
    if (checkPauseOrExit()) return;
    
    progress.store(0.7);
    
    analyserKey->analyse(buffer, track.key);
    
    progress.store(0.8);
    
    analyserGroove->analyse(buffer, track.groove);
    
    progress.store(0.9);
    
    if (checkPauseOrExit()) return;
    
    track.analysed = true;
    
    dataManager->releaseAudio(buffer);
    
    if (checkPauseOrExit()) return;
    
    analysisManager->storeAnalysis(&track);
    
    progress.store(1.0);
}


bool AnalysisThread::checkPauseOrExit()
{
    // If analysis is paused, and there is no exit request, sleep for 1 second
    while (pause.load() && !threadShouldExit())
        sleep(1000);
    
    // If there is an exit request, return true
    if (threadShouldExit())
        return true;
    // Otherwise, return false
    return false;
}
