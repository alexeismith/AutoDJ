//
//  AnalyserKey.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 14/04/2021.
//

#ifndef AnalyserKey_hpp
#define AnalyserKey_hpp

#include <JuceHeader.h>
#include "ThirdParty/qm-dsp/dsp/keydetection/GetKeyMode.h"


/**
 Tonal MIR analyser that uses QM-DSP algorithm to extract key signature, based on: https://www.aes.org/e-lib/browse.cfm?elib=14140
 Implementation is inspired by: https://github.com/c4dm/qm-vamp-plugins/blob/master/plugins/KeyDetect.cpp
*/
class AnalyserKey
{
public:
    
    /** Constructor. */
    AnalyserKey() {}
    
    /** Destructor. */
    ~AnalyserKey() {}
    
    /** Analyses the provided audio data.
     
     @param[in] audio Pointer to audio data to be analysed
     @param[out] key Output location for key signature result */
    void analyse(juce::AudioBuffer<float>* audio, int& key);
    
private:
    
    /** Resets the analyser ready for a new track. */
    void reset();
    
    std::unique_ptr<GetKeyMode> keyDetector; ///< QM-DSP key signature detector
    
    juce::Array<int> keys; ///< Intermediate storage for all keys detected within a single track
//    juce::Array<int> changes; ///< Intermediate storage for any locations where the key changes within a single track (unused)
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalyserKey) ///< JUCE macro to add a memory leak detector
};

#endif /* AnalyserKey_hpp */
