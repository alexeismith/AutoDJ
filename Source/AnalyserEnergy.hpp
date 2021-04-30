//
//  AnalyserEnergy.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 30/04/2021.
//

#ifndef AnalyserEnergy_hpp
#define AnalyserEnergy_hpp

#include <JuceHeader.h>

#include <essentia.h>
#include <algorithmfactory.h>


class AnalyserEnergy
{
public:
    
    AnalyserEnergy(essentia::standard::AlgorithmFactory& factory);
    
    ~AnalyserEnergy() {}
    
    void analyse(juce::AudioBuffer<float>* audio, int& energy);
    
private:
    
    void reset();
    
    std::unique_ptr<essentia::standard::Algorithm> danceability;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalyserEnergy)
};

#endif /* AnalyserEnergy_hpp */
