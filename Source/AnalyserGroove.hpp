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


/**
 MIR analyser that uses Essentia algorithm to extract 'danceability', which we call 'groove'
 
 Algorithm is based on: http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.73.8209
*/
class AnalyserGroove
{
public:
    
    /** Constructor. */
    AnalyserGroove(essentia::standard::AlgorithmFactory& factory);
    
    /** Destructor. */
    ~AnalyserGroove() {}
    
    /** Analyses the provided audio data.
    
     @param[in] audio Pointer to audio data to be analysed
     @param[out] groove Output location for groove result */
    void analyse(juce::AudioBuffer<float>* audio, float& groove);
    
private:
    
    /** Resets the analyser ready for a new track. */
    void reset();
    
    std::unique_ptr<essentia::standard::Algorithm> danceability; ///< Essentia danceability algorithm
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalyserGroove) ///< JUCE macro to add a memory leak detector
};

#endif /* AnalyserGroove_hpp */
