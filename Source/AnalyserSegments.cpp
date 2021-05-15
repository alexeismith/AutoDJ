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
    filter.setCoefficients(juce::IIRCoefficients::makeLowPass(SUPPORTED_SAMPLERATE, 400, 1.0));
}


juce::Array<int> AnalyserSegments::analyse(TrackInfo* track, juce::AudioBuffer<float>* audio)
{
    reset();
    
    int numSamples = audio->getNumSamples();
    
    filteredBuffer.setSize(1, numSamples);
    
    filteredBuffer.copyFrom(0, 0, audio->getReadPointer(0), numSamples);
    
    if (audio->getNumChannels() == 2)
    {
        filteredBuffer.copyFrom(0, 0, audio->getReadPointer(1), numSamples);
        filteredBuffer.applyGain(0.5f);
    }
    
    filter.processSamples(filteredBuffer.getWritePointer(0), numSamples);
    
    int windowSize = segmenter->getWindowsize();
    
    double *tempBuffer = new double[windowSize];
    
    int numFrames = numSamples / windowSize;
    
    for (int i = 0; i < numFrames; i++)
    {
        for (int j = 0; j < windowSize; j++)
        {
            tempBuffer[j] = (double)filteredBuffer.getReadPointer(0)[i*windowSize + j];
        }

        segmenter->extractFeatures(tempBuffer, segmenter->getWindowsize());
    }
    
    segmenter->segment(NUM_SEGMENT_TYPES);
    Segmentation segmentation = segmenter->getSegmentation();
    
    juce::Array<int> segments;
    int segmentAdjust;
    
    for (auto segment : segmentation.segments)
    {
        segmentAdjust = track->getNearestDownbeat(segment.start * 3);
        segments.add(segmentAdjust);
    }
    
    delete [] tempBuffer;
    
    return segments;
}


int AnalyserSegments::findClosestSegment(TrackInfo* track, juce::Array<int>* segments, int sample, int min, int max)
{
    int segmentBelow, segmentAbove, closest = -1;

    // Ensure the provided sample is within the provided range
    sample = juce::jlimit(min, max, sample);

    // If there are no track segments, return the nearest downbeat
    if (segments->isEmpty())
        return track->getNearestDownbeat(sample);
    
    // Check whether any segments are in range
    bool inRange = false;
    for (int segment : *segments)
    {
        if (segment > min && segment < max)
        {
            inRange = true;
            break;
        }
    }
    // If none are in range, return the nearest downbeat
    if (!inRange)
        return track->getNearestDownbeat(sample);
    
    // If sample is below the first segment or above the last, return those
    if (sample < segments->getFirst() || segments->size() == 1)
        return segments->getFirst();
    else if (sample > segments->getLast())
        return segments->getLast();

    // Loop through the segments to find the nearest one
    for (int i = 0; i < segments->size()-1; i++)
    {
        segmentBelow = segments->getUnchecked(i);
        segmentAbove = segments->getUnchecked(i+1);
        
        // If sample is between the current segments, find which is closest
        if (sample > segmentBelow && sample < segmentAbove)
        {
            // If one segment isn't in range, and the other is, choose that one...
            
            if (segmentBelow < min)
            {
                closest = segmentAbove;
                break;
            }
            else if (segmentAbove > max)
            {
                closest = segmentBelow;
                break;
            }
            
            // Check whether this one or the segment above is closer...

            if (abs(sample - segmentBelow) < abs(sample - segmentAbove))
                closest = segmentBelow;
            else
                closest = segmentAbove;

            // Break out of the loop
            break;
        }
    }

    if (closest == -1)
    {
        jassert(false); // Closest segment is still not found!
        return track->getNearestDownbeat(sample);
    }
    
    jassert(closest < max && closest > min);

    return closest;
}


bool AnalyserSegments::isSegment(juce::Array<int>* segments, int sample)
{
    for (int segment : *segments)
        if (segment == sample)
            return true;
    
    return false;
}


void AnalyserSegments::reset()
{
    segmenter.reset(new ClusterMeltSegmenter(params));
    segmenter->initialise(SUPPORTED_SAMPLERATE);
    
    filter.reset();
    filteredBuffer.clear();
}
