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
    
    const int maxInputOutputRatio = 4;
    
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
    double ratio = shifter.getInputOutputSampleRatio();
    int numInput = round(double(numSamples) / ratio);
    
#ifdef STRETCHER_MONO
    jassert(numInput <= inputInterleaved.getNumSamples());
    jassert(numSamples <= outputInterleaved.getNumSamples());
#else
    jassert(numInput*2 <= inputInterleaved.getNumSamples());
    jassert(numSamples*2 <= outputInterleaved.getNumSamples());
#endif
    
    while (shifter.numSamples() < numSamples)
    {
        if (playhead >= input->getNumSamples() - 1)
        {
            shifter.flush();
            break;
        }
        else if (playhead >= input->getNumSamples() - numInput)
        {
            numInput = input->getNumSamples() - playhead - 1;
        }
        
        interleave(input, numInput);
        
        shifter.putSamples(inputInterleaved.getReadPointer(0), numInput);
        
        playhead += numInput;
    }
    
    shifter.receiveSamples(outputInterleaved.getWritePointer(0), numSamples);
    
    deinterleave(output, numSamples);
    
    return numInput;
}


void TimeStretcher::update(double bpmOriginal, double bpmTarget)
{
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
    
    for (int i = 0; i < numSamples; i++)
    {
        output->setSample(0, i, outputInterleaved.getSample(0, i*2));
        output->setSample(1, i, outputInterleaved.getSample(0, i*2 + 1));
    }
#endif
}
