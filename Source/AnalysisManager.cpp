//
//  AnalysisManager.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 26/03/2021.
//

#include "AnalysisManager.hpp"

#include "../../Third Party/qm-dsp-1.7.1/dsp/tempotracking/TempoTrackV2.h"

void AnalysisManager::analyse(TrackData track)
{
    juce::AudioBuffer<float> buffer;
    
    dataManager->fetchAudio(track.filename, buffer, true);
}
