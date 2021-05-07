//
//  AnalyserSegments.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 07/05/2021.
//

#include "AnalyserSegments.hpp"

#include "CommonDefs.hpp"

#define NUM_SEGMENT_TYPES (10)


AnalyserSegments::AnalyserSegments()
{
    params.ncomponents = 20;
    
    int minimumSegmentDuration = 4;

    params.neighbourhoodLimit = int(minimumSegmentDuration / params.hopSize + 0.0001);
    filter.setCoefficients(juce::IIRCoefficients::makeLowPass(SUPPORTED_SAMPLERATE, 600, 1.0));
}


juce::Array<int> AnalyserSegments::analyse(juce::AudioBuffer<float>* audio)
{
    reset();
    
//    filter.processSamples(audio->getWritePointer(1), audio->getNumSamples());
    
    int windowSize = segmenter->getWindowsize();
    
    double *tempBuffer = new double[windowSize];
    
    int numFrames = audio->getNumSamples() / windowSize;
    
    for (int i = 0; i < numFrames; i++)
    {
        for (int j = 0; j < windowSize; j++)
        {
            tempBuffer[j] = (double)audio->getReadPointer(0)[i*windowSize + j];
        }

        segmenter->extractFeatures(tempBuffer, segmenter->getWindowsize());
    }

    delete [] tempBuffer;
    
    segmenter->segment(NUM_SEGMENT_TYPES);
    Segmentation segmentation = segmenter->getSegmentation();
    
    juce::Array<int> segments;
    
    for (auto segment : segmentation.segments)
        segments.add(segment.start * 3);
    
    return segments;
}


void AnalyserSegments::reset()
{
    segmenter.reset(new ClusterMeltSegmenter(params));
    segmenter->initialise(SUPPORTED_SAMPLERATE);
    
    filter.reset();
}
