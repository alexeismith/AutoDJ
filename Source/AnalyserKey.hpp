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

class AnalyserKey
{
public:
    
    AnalyserKey() {}
    
    ~AnalyserKey() {}
    
    void analyse(juce::AudioBuffer<float>* audio, int& key);
    
private:
    
    void reset();
    
    std::unique_ptr<GetKeyMode> keyDetector;
    
    juce::Array<int> keys;
    juce::Array<int> changes;
    
    int prevKey;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalyserKey) ///< JUCE macro to add a memory leak detector
};

#endif /* AnalyserKey_hpp */
