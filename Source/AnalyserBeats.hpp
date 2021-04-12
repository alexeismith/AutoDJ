//
//  AnalyserBeats.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 11/04/2021.
//

#ifndef AnalyserBeats_hpp
#define AnalyserBeats_hpp

#include <JuceHeader.h>

#include "ThirdParty/qm-dsp/dsp/tempotracking/TempoTrackV2.h"
#include "ThirdParty/qm-dsp/dsp/onsets/DetectionFunction.h"

class AnalyserBeats
{
public:
    
    AnalyserBeats();
    
    ~AnalyserBeats() {}
    
    void analyse(juce::AudioBuffer<float> audio, int& bpm, int& beatPhase, int& downbeat);
    
private:
    
    void processBeats(std::vector<double> beats, int& bpm, int& beatPhase, int& downbeat);
    
    TempoTrackV2 tempoTracker;
    std::unique_ptr<DetectionFunction> onsetAnalyser;
    DFConfig dfConfig;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalyserBeats)
};

#endif /* AnalyserBeats_hpp */
