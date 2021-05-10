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
    shifter.setSampleRate(SUPPORTED_SAMPLERATE);
    shifter.setChannels(1);
    shifter.clear();
}


void TimeStretcher::prepare(int blockSize)
{
//    buffer.setSize(1, blockSize);
}


int TimeStretcher::process(juce::AudioBuffer<float>* input, juce::AudioBuffer<float>* output, int numSamples)
{
    double ratio = shifter.getInputOutputSampleRatio();
    int numInput = round(double(numSamples) / ratio);
    
    while (shifter.numSamples() < numSamples)
    {
        if (playhead >= input->getNumSamples() - numInput)
            return 0; // TODO: handle this better
        
        shifter.putSamples(input->getReadPointer(0, playhead), numInput);
        playhead += numInput;
    }
    
    shifter.receiveSamples(output->getWritePointer(0), numSamples);
    
    return numInput;
}


void TimeStretcher::update(double bpmOriginal, double bpmTarget)
{
    timeStretch = bpmTarget / bpmOriginal;
    shifter.setTempo(timeStretch);
}


void TimeStretcher::resetPlayhead(int sample)
{
    reset();
    playhead = sample;
}


void TimeStretcher::reset()
{
    shifter.clear();
}
