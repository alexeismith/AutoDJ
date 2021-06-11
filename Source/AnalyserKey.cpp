//
//  AnalyserKey.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 14/04/2021.
//

#include "AnalyserKey.hpp"

#include "CommonDefs.hpp"

#define TUNING_FREQUENCY_HZ (440)


void AnalyserKey::analyse(juce::AudioBuffer<float>* audio, int& key)
{
    int currentKey, windowSize, hopSize, numFrames;
    juce::AudioBuffer<double> buffer;
    
    // Reset the analyser ready for the new audio
    reset();
    
    // Fetch the size of the analyser frames and the distance between them
    windowSize = keyDetector->getBlockSize();
    hopSize = keyDetector->getHopSize();
    
    // Find the number of audio frames to be processed
    numFrames = (audio->getNumSamples() - windowSize) / hopSize;
    
    // Analysis classes require double, so copy audio into a double buffer
    buffer.setSize(1, audio->getNumSamples());
    for (int i = 0; i < audio->getNumSamples(); i++)
        buffer.setSample(0, i, (double)audio->getSample(0, i));
    
    // For each analysis frame
    for (int i = 0; i < numFrames; i++)
    {
        // Perform key signature detection
        currentKey = keyDetector->process(buffer.getWritePointer(0));
        
        // If the result is not the same as the previous frame...
        if (currentKey != keys.getLast() || i == 0)
        {
            // Add it to the keys array
            keys.add(currentKey);
            // Could store key change locations, but we don't need to
//            changes.add(i * hopSize);
        }
    }
    
    // Find the most common key that was detected
    key = AutoDJ::mostCommonValue(keys.data(), keys.size());
}


void AnalyserKey::reset()
{
    keyDetector.reset(new GetKeyMode(GetKeyMode::Config(SUPPORTED_SAMPLERATE, TUNING_FREQUENCY_HZ)));
}
