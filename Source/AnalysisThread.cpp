
//
//  AnalysisThread.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 18/04/2021.
//

#include "AnalysisThread.hpp"

#include "AnalysisManager.hpp"


AnalysisThread::AnalysisThread(int ID, AnalysisManager* am, TrackDataManager* dm) :
    juce::Thread("Parser"), id(ID), analysisManager(am), dataManager(dm)
{
    analyserBeats.reset(new AnalyserBeats());
    analyserKey.reset(new AnalyserKey());
}


void AnalysisThread::run()
{
    bool finished;
    
    TrackData track = analysisManager->getNextJob(finished);
    
    while (!finished)
    {
        analyse(track);
        track = analysisManager->getNextJob(finished);
    }
    
    DBG("Analysis Thread " << id << " Finished");
}


void AnalysisThread::analyse(TrackData& track)
{
    juce::AudioBuffer<float> buffer;
    
    DBG("Analysis Thread " << id << ": " << track.filename);
    
    dataManager->fetchAudio(track.filename, buffer, true);

    analyserBeats->analyse(buffer, track.bpm, track.beatPhase, track.downbeat);
    
    analyserKey->analyse(buffer, track.key);
    
    track.analysed = true;
    
    dataManager->update(track);
}
