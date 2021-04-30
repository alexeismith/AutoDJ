//
//  AnalyserGroove.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 30/04/2021.
//

#include "AnalyserGroove.hpp"

#include "CommonDefs.hpp"


AnalyserGroove::AnalyserGroove(essentia::standard::AlgorithmFactory& factory)
{
    danceability.reset(factory.create("Danceability", "sampleRate", SUPPORTED_SAMPLERATE));
}


void AnalyserGroove::analyse(juce::AudioBuffer<float>* audio, float& groove)
{
    std::vector<essentia::Real> buffer(audio->getReadPointer(0), audio->getReadPointer(0) + audio->getNumSamples());
    std::vector<essentia::Real> tempBuffer;
    
    danceability->reset();
    
    danceability->input("signal").set(buffer);
    danceability->output("danceability").set(groove);
    danceability->output("dfa").set(tempBuffer);
    danceability->compute();
}
