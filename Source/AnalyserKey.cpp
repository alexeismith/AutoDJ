//
//  AnalyserKey.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 14/04/2021.
//

#include "AnalyserKey.hpp"

#include "CommonDefs.hpp"

#define TUNING_FREQUENCY_HZ (440)


void AnalyserKey::analyse(juce::AudioBuffer<float> audio, int& key)
{
    int currentKey, windowSize, hopSize, numFrames;
    juce::AudioBuffer<double> buffer;
    
    reset();
    
    windowSize = keyDetector->getBlockSize();
    hopSize = keyDetector->getHopSize();
    
    numFrames = (audio.getNumSamples() - windowSize) / hopSize;
    
    // Analysis classes require double, so copy audio into a double buffer
    buffer.setSize(1, audio.getNumSamples());
    for (int i = 0; i < audio.getNumSamples(); i++)
        buffer.setSample(0, i, (double)audio.getSample(0, i));
    
    for (int i = 0; i < numFrames; i++)
    {
        currentKey = keyDetector->process(buffer.getWritePointer(0));
        
        if (currentKey != prevKey || i == 0)
        {
            keys.add(currentKey);
            changes.add(i * hopSize);
        }
        
        prevKey = currentKey;
    }
    
    key = AutoDJ::mostCommonValue(keys.data(), keys.size());
}


void AnalyserKey::reset()
{
    keyDetector.reset(new GetKeyMode(SUPPORTED_SAMPLERATE, TUNING_FREQUENCY_HZ, 10, 10));
    prevKey = -1;
}
