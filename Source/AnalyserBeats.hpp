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
#include "ThirdParty/qm-dsp/dsp/tempotracking/DownBeat.h"
#include "ThirdParty/qm-dsp/dsp/onsets/DetectionFunction.h"

class AnalyserBeats
{
public:
    
    /** Constructor. */
    AnalyserBeats();
    
    /** Destructor. */
    ~AnalyserBeats() {}
    
    void analyse(juce::AudioBuffer<float>* audio, std::atomic<double>* progress, int& bpm, int& beatPhase, int& downbeat);
    
private:
    
    void reset();
    
    void getTempo(juce::AudioBuffer<float>* audio, std::atomic<double>* progress, int numFrames, int& bpm, int& beatPhase);
    
    void processBeats(std::vector<double> beats, int& bpm, int& beatPhase);
    
    void getDownbeat(juce::AudioBuffer<float>* audio, int numFrames, int bpm, int beatPhase, int& downbeat);
    
    bool isBeat(int frame, int bpm, int beatPhase);
    
    std::unique_ptr<DetectionFunction> onsetAnalyser;
    std::unique_ptr<DownBeat> downBeat;
    DFConfig dfConfig;
    
    juce::IIRFilter filter;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalyserBeats) ///< JUCE macro to add a memory leak detector
};

#endif /* AnalyserBeats_hpp */
