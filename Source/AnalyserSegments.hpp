//
//  AnalyserSegments.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 07/05/2021.
//

#ifndef AnalyserSegments_hpp
#define AnalyserSegments_hpp

#include <JuceHeader.h>

#include "TrackInfo.hpp"

#include "ThirdParty/qm-dsp/dsp/segmentation/ClusterMeltSegmenter.h"


class AnalyserSegments
{
public:
    
    AnalyserSegments();
    
    ~AnalyserSegments() {}
    
    juce::Array<int> analyse(TrackInfo* track, juce::AudioBuffer<float>* audio);
    
    int findClosestSegment(TrackInfo* track, juce::Array<int>* segments, int sample, int min, int max);
    
    bool isSegment(juce::Array<int>* segments, int sample);
    
private:
    
    void reset();
    
    std::unique_ptr<Segmenter> segmenter;
    ClusterMeltSegmenterParams params;
    
    juce::AudioBuffer<float> filteredBuffer;
    
    juce::IIRFilter filter;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalyserSegments) ///< JUCE macro to add a memory leak detector
};

#endif /* AnalyserSegments_hpp */
