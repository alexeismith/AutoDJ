//
//  TimeStretcher.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 10/05/2021.
//

#include "TimeStretcher.hpp"

#include "CommonDefs.hpp"


TimeStretcher::TimeStretcher()
{
    // Set the samplerate of SoundTouch using the
    // globally-supported sample rate
    shifter.setSampleRate(SUPPORTED_SAMPLERATE);
    
    // Set the number of channels depending on
    // the pre-processor mono/stereo macro
#ifdef STRETCHER_MONO
    shifter.setChannels(1);
#else
    shifter.setChannels(2);
#endif
    
    reset();
}


void TimeStretcher::prepare(int blockSize)
{
    inputInterleaved.clear();
    
    // Define the maximum ratio of input samples to output samples - this is affected by time stretch.
    // Effectively, we are saying the maximum stretch is a factor of 4
    const int maxInputOutputRatio = 4;
    
    // Allocate audio buffer space for this maximum stretch
#ifdef STRETCHER_MONO
    inputInterleaved.setSize(1, blockSize * maxInputOutputRatio);
    outputInterleaved.setSize(1, blockSize);
#else
    inputInterleaved.setSize(1, blockSize * maxInputOutputRatio * 2);
    outputInterleaved.setSize(1, blockSize * 2);
#endif
}


int TimeStretcher::process(juce::AudioBuffer<float>* input, juce::AudioBuffer<float>* output, int numSamples)
{
    // Find the number of input samples that correspond to the requested number of output samples
    double ratio = shifter.getInputOutputSampleRatio();
    int numInput = round(double(numSamples) / ratio);
    
    // Ensure we have space in the processing buffers for this
    // (Throw a debug error if not)
#ifdef STRETCHER_MONO
    jassert(numInput <= inputInterleaved.getNumSamples());
    jassert(numSamples <= outputInterleaved.getNumSamples());
#else
    jassert(numInput*2 <= inputInterleaved.getNumSamples());
    jassert(numSamples*2 <= outputInterleaved.getNumSamples());
#endif
    
    // The SoundTouch library has processing latency, meaning extra samples have be given in order to receive a certain number of output samples
    // Soundtouch::numSamples() returns the number of samples that the library has ready
    // So, loop until there are enough output samples ready
    while (shifter.numSamples() < numSamples)
    {
        // If the input playhead reached the end
        if (playhead >= input->getNumSamples() - 1)
        {
            // Flush the final output samples out of SoundTouch and break this loop
            shifter.flush();
            break;
        }
        // Or, if there is a shortage of input samples left
        else if (playhead >= input->getNumSamples() - numInput)
        {
            // Adjust the numInput variable to match the number left
            numInput = input->getNumSamples() - playhead - 1;
        }
        
        // Interleave the stereo samples into a single buffer to give to SoundTouch
        interleave(input, numInput);
        
        // Send the audio to SoundTouch for stretching
        shifter.putSamples(inputInterleaved.getReadPointer(0), numInput);
        
        // Increment the input audio playhead to keep track of how many samples have been passed to SoundTouch
        playhead += numInput;
    }
    
    // Fetch the output samples from SoundTouch
    shifter.receiveSamples(outputInterleaved.getWritePointer(0), numSamples);
    
    // Detinterleave them back into stereo audio channels
    deinterleave(output, numSamples);
    
    // Return the number of input samples that correspond to the stretched output
    return numInput;
}


void TimeStretcher::update(double bpmOriginal, double bpmTarget)
{
    // Calculate time stretch factor, where 1.0 is original tempo, >1.0 is faster, and <1.0 is slower
    // It is simply target BPM divided by original BPM
    timeStretch.store(bpmTarget / bpmOriginal);
    shifter.setTempo(timeStretch.load());
}


void TimeStretcher::resetPlayhead(int sample)
{
    reset();
    playhead = sample;
    
    shifter.setRate(1.0);
    shifter.setPitch(1.0);
    shifter.setTempo(1.0);
}


void TimeStretcher::reset()
{
    shifter.clear();
    inputInterleaved.clear();
    outputInterleaved.clear();
}


void TimeStretcher::interleave(juce::AudioBuffer<float>* input, int numSamples)
{
#ifdef STRETCHER_MONO
    inputInterleaved.copyFrom(0, 0, *input, 0, playhead, numSamples);
#else
    jassert(input->getNumChannels() >= 2); // Input must be stereo!
    
    // Place the stereo samples side by side in a single buffer
    for (int i = 0; i < numSamples; i++)
    {
        inputInterleaved.setSample(0, i*2, input->getSample(0, playhead + i));
        inputInterleaved.setSample(0, i*2 + 1, input->getSample(1, playhead + i));
    }
#endif
}


void TimeStretcher::deinterleave(juce::AudioBuffer<float>* output, int numSamples)
{
#ifdef STRETCHER_MONO
    output->copyFrom(0, 0, outputInterleaved.getReadPointer(0), numSamples);
    output->copyFrom(1, 0, outputInterleaved.getReadPointer(0), numSamples);
#else
    jassert(output->getNumChannels() >= 2); // Output must be stereo!
    
    // Copy the side-by-side stereo samples into separate L&R buffer channels
    for (int i = 0; i < numSamples; i++)
    {
        output->setSample(0, i, outputInterleaved.getSample(0, i*2));
        output->setSample(1, i, outputInterleaved.getSample(0, i*2 + 1));
    }
#endif
}
