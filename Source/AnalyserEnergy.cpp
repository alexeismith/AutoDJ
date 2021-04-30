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


void AnalyserEnergy::analyse(juce::AudioBuffer<float>* audio, int& energy)
{
    
}
