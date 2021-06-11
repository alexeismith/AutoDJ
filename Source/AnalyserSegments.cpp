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
    
    // Set low-pass filter parameters (400Hz cut-off, 1.0 Q)
    filter.setCoefficients(juce::IIRCoefficients::makeLowPass(SUPPORTED_SAMPLERATE, 400, 1.0));
}


juce::Array<int> AnalyserSegments::analyse(TrackInfo* track, juce::AudioBuffer<float>* audio)
{
    // Reset ready for the new track
    reset();
    
    // Resize the filter audio buffer
    int numSamples = audio->getNumSamples();
    filteredBuffer.setSize(1, numSamples);
    
    // Copy the input audio into the filtered buffer
    filteredBuffer.copyFrom(0, 0, audio->getReadPointer(0), numSamples);
    // If the audio is stereo, also copy the second channel
    if (audio->getNumChannels() == 2)
    {
        filteredBuffer.addFrom(0, 0, audio->getReadPointer(1), numSamples);
        filteredBuffer.applyGain(0.5f); // Attenuate volume to account for added channel
    }
    
    // Low-pass the audio
    filter.processSamples(filteredBuffer.getWritePointer(0), numSamples);
    
    // Analyser requires double, so copy audio into a double buffer
    // (Filter can't operate on doubles)
    juce::AudioBuffer<double> buffer;
    buffer.setSize(1, filteredBuffer.getNumSamples());
    for (int i = 0; i < filteredBuffer.getNumSamples(); i++)
        buffer.setSample(0, i, (double)filteredBuffer.getSample(0, i));
    
    // Find the number of analysis frames
    int windowSize = segmenter->getWindowsize();
    int hopSize = segmenter->getHopsize();
    int numFrames = (numSamples - windowSize) / hopSize;
    
    // Pass each frame of filtered audio to the segmentation algorithm
    for (int i = 0; i < numFrames; i++)
        segmenter->extractFeatures(buffer.getReadPointer(0, i*hopSize), windowSize);
    
    // Trigger the segmentation
    segmenter->segment(NUM_SEGMENT_TYPES);
    // Fetch the result
    Segmentation segmentation = segmenter->getSegmentation();
    
    // We're only interested in the segment starts
    // Create a new array to store these
    juce::Array<int> segments;
    for (auto segment : segmentation.segments)
        // Lock each segment to a downbeat
        segments.add(track->getNearestDownbeat(segment.start));
    
    // Return the array
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
