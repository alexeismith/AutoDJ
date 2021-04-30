//
//  AnalyserEnergy.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 30/04/2021.
//

#include "AnalyserEnergy.hpp"

#include "CommonDefs.hpp"


AnalyserEnergy::AnalyserEnergy(essentia::standard::AlgorithmFactory& factory)
{
    danceability.reset(factory.create("Danceability", "sampleRate", SUPPORTED_SAMPLERATE));
}


void AnalyserEnergy::analyse(juce::AudioBuffer<float>* audio, float& energy)
{
    std::vector<essentia::Real> buffer(audio->getReadPointer(0), audio->getReadPointer(0) + audio->getNumSamples());
    std::vector<essentia::Real> tempBuffer;
    
    danceability->reset();
    
    danceability->input("signal").set(buffer);
    danceability->output("danceability").set(energy);
    danceability->output("dfa").set(tempBuffer);
    danceability->compute();
}
