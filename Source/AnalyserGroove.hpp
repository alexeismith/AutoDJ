//
//  AnalyserGroove.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 30/04/2021.
//

#ifndef AnalyserGroove_hpp
#define AnalyserGroove_hpp

#include <JuceHeader.h>
#include <essentia.h>
#include <algorithmfactory.h>


class AnalyserGroove
{
public:
    
    /** Constructor. */
    AnalyserGroove(essentia::standard::AlgorithmFactory& factory);
    
    /** Destructor. */
    ~AnalyserGroove() {}
    
    void analyse(juce::AudioBuffer<float>* audio, float& groove);
    
private:
    
    void reset();
    
    std::unique_ptr<essentia::standard::Algorithm> danceability;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalyserGroove) ///< JUCE macro to add a memory leak detector
};

#endif /* AnalyserGroove_hpp */
