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


/**
 MIR analyser that uses QM-DSP algorithm to find distinct musical sections in a track, based on: https://doi.org/10.1109/TASL.2007.910781
 
 Implementation is inspired by: https://github.com/c4dm/qm-vamp-plugins/blob/master/plugins/SegmenterPlugin.cpp
 - A significant change is that the input audio is low-pass filtered here, because sections in dance music can be found more effective by focusing on bass frequencies (this still needs formal testing)
*/
class AnalyserSegments
{
public:
    
    /** Constructor. */
    AnalyserSegments();
    
    /** Destructor. */
    ~AnalyserSegments() {}
    
    /** Analyses the provided audio data.
    
     @param[in] track Pointer to track information, so that segments can be matched to the nearest downbeat
     @param[in] audio Pointer to audio data to be analysed
     
     @return Array of musical segment boundaries, measured in audio samples */
    juce::Array<int> analyse(TrackInfo* track, juce::AudioBuffer<float>* audio);
    
    /** Fetches the location of the segment closest to a given audio sample point.
     Can optionally provide a range in which to restrict the result.
     
     @param[in] track Pointer to track information, so that nearest downbeat can be returned if there are no segments within range
     @param[in] segments Array of musical segment boundaries, measured in audio samples
     @param[in] sample Audio sample position to check against
     @param[in] min Minimum audio sample that could be returned
     @param[in] min Maximum audio sample that could be returned
     
     @return Nearest segment to the input sample, or the nearest downbeat if no segments in range */
    int findClosestSegment(TrackInfo* track, juce::Array<int>* segments, int sample, int min, int max);
    
    /** Checks whether a given audio sample is in the provided segments array.
     
     @param[in] segments Array of musical segment boundaries, measured in audio samples
     @param[in] sample Audio sample position to check against
     
     @return Result of the check */
    bool isSegment(juce::Array<int>* segments, int sample);
    
private:
    
    /** Resets the analyser ready for a new track. */
    void reset();
    
    std::unique_ptr<Segmenter> segmenter; ///< QM-DSP segmentation algorithm
    ClusterMeltSegmenterParams params; ///< Parameters for QM-DSP segmentation algorithm
    
    juce::IIRFilter filter; ///< IIR filter for low-passing input audio
    juce::AudioBuffer<float> filteredBuffer; ///< Intermediate audio buffer for low-pass filtered audio
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalyserSegments) ///< JUCE macro to add a memory leak detector
};

#endif /* AnalyserSegments_hpp */
