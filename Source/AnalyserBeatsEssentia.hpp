//
//  AnalyserBeatsEssentia.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 11/04/2021.
//

#ifndef AnalyserBeatsEssentia_hpp
#define AnalyserBeatsEssentia_hpp

#include <JuceHeader.h>

#include "ThirdParty/qm-dsp/dsp/tempotracking/DownBeat.h"

#include <essentia.h>
#include <algorithmfactory.h>

class AnalyserBeatsEssentia
{
public:
    
    AnalyserBeatsEssentia(essentia::standard::AlgorithmFactory& factory);
    
    ~AnalyserBeatsEssentia() {}
    
    void analyse(juce::AudioBuffer<float>* audio, std::atomic<double>* progress, int& bpm, int& beatPhase, int& downbeat);
    
private:
    
    void reset();
    
    void getTempo(juce::AudioBuffer<float>* audio, std::atomic<double>* progress, int& bpm, int& beatPhase);
    
    void processBeats(std::vector<double> beats, int bpm, int& beatPhase);
    
    void pulseTrainsPhase(juce::AudioBuffer<float>* audio, int bpm, int& beatPhase);
    
    void getDownbeat(juce::AudioBuffer<float>* audio, int bpm, int beatPhase, int& downbeat);
    
    bool isBeat(int frame, int bpm, int beatPhase);
    
    std::unique_ptr<essentia::standard::Algorithm> rhythmExtractor;
    std::unique_ptr<essentia::standard::Algorithm> percivalTempo;
    std::unique_ptr<essentia::standard::Algorithm> percivalPulseTrains;
    std::unique_ptr<essentia::standard::Algorithm> onsetGlobal;
    
    std::unique_ptr<DownBeat> downBeat;
    
    juce::AudioBuffer<float> filteredBuffer;
    
    juce::IIRFilter filter;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalyserBeatsEssentia)
};

#endif /* AnalyserBeatsEssentia_hpp */
