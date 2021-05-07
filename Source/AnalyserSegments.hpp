//
//  AnalyserSegments.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 07/05/2021.
//

#ifndef AnalyserSegments_hpp
#define AnalyserSegments_hpp

#include <JuceHeader.h>

#include "ThirdParty/qm-dsp/dsp/segmentation/ClusterMeltSegmenter.h"


class AnalyserSegments
{
public:
    
    AnalyserSegments();
    
    ~AnalyserSegments() {}
    
    juce::Array<int> analyse(juce::AudioBuffer<float>* audio);
    
private:
    
    void reset();
    
    std::unique_ptr<Segmenter> segmenter;
    ClusterMeltSegmenterParams params;
    
    juce::IIRFilter filter;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalyserSegments)
};

#endif /* AnalyserSegments_hpp */
