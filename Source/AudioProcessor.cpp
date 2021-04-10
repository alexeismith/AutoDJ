//
//  AudioProcessor.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 09/04/2021.
//

#include "AudioProcessor.hpp"


AudioProcessor::AudioProcessor()
{
    audioReady.store(false);
}


void AudioProcessor::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    
    if (!audioReady.load()) return;
    
    int numSamples = bufferToFill.numSamples;
    
    if (playhead > -1 && playhead < (audioBuffer.getNumSamples() - numSamples))
    {
        memcpy(bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample), audioBuffer.getReadPointer(0, playhead), numSamples*sizeof(float));
        memcpy(bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample), audioBuffer.getReadPointer(0, playhead), numSamples*sizeof(float));
        
        playhead += numSamples;
    }
    else
    {
        playhead = -1;
    }
    
}


void AudioProcessor::play(juce::AudioBuffer<float> audio)
{
    audioReady.store(false);
    
    audioBuffer = audio;
    playhead = 0;
    
    audioReady.store(true);
}
