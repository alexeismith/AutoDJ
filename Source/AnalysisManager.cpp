//
//  AnalysisManager.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 26/03/2021.
//

#include "AnalysisManager.hpp"

AnalysisManager::AnalysisManager(TrackDataManager* dm) :
    dataManager(dm)
{
    analyserBeats.reset(new AnalyserBeats());
}


void AnalysisManager::analyse(TrackData& track)
{
    juce::AudioBuffer<float> buffer;
    
    DBG("Analysing " << track.filename);
    
    dataManager->fetchAudio(track.filename, buffer, true);

    analyserBeats->analyse(buffer, track.bpm, track.beatPhase, track.downbeat);
    
    dataManager->update(track);
    
    DBG("Analysis Done");
}
