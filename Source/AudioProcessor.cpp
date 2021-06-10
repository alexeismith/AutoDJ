//
//  AudioProcessor.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 09/04/2021.
//

#include "AudioProcessor.hpp"

#include "CommonDefs.hpp"


AudioProcessor::AudioProcessor(DataManager* dataManager, ArtificialDJ* dj, int initBlockSize)
{
    trackProcessors.add(new TrackProcessor(dataManager, dj));
    trackProcessors.add(new TrackProcessor(dataManager, dj));
    
    getTrackProcessor(0)->setPartner(getTrackProcessor(1));
    getTrackProcessor(1)->setPartner(getTrackProcessor(0));
    
    prepare(initBlockSize);
}


void AudioProcessor::getNextAudioBlock(const juce::AudioSourceChannelInfo& outputBuffer)
{
    TrackProcessor* leader = nullptr;
    TrackProcessor* follower = nullptr;
    
    outputBuffer.clearActiveBufferRegion();
    
    // If a skip has been requested, skip to the next mix event
    if (skipFlag.load())
        skipToNextEvent();
    
    // If audio is paused, return
    if (paused.load())
        return;
    
    // Find which is the leading track processor (i.e. the one playing the current track)
    getTrackProcessors(&leader, &follower);
    // If no leader was found, throw a debug error
    if (!leader)
        jassert(false); // No leader!
    
    // Process audio through the leader and follower (order here doesn't matter)
    leader->getNextAudioBlock(outputBuffer);
    follower->getNextAudioBlock(outputBuffer);
    
    // Check whether the follower should start playing, if it isn't already
    follower->cue(leader->getPlayheadPosition());
    
    // If the output volume isn't at the target set in the UI, ramp to the target value over the duration of the audio buffer
    if (volume != targetVolume.load())
    {
        outputBuffer.buffer->applyGainRamp(outputBuffer.startSample, outputBuffer.numSamples, volume, targetVolume.load());
        volume = targetVolume.load();
    }
    else // Otherwise, just apply the current output volume
    {
        outputBuffer.buffer->applyGain(outputBuffer.startSample, outputBuffer.numSamples, volume);
    }
}


void AudioProcessor::getTrackProcessors(TrackProcessor** leader, TrackProcessor** follower)
{
    for (auto* processor : trackProcessors)
    {
        if (processor->isLeader())
            *leader = processor;
        else
            *follower = processor;
    }
}


void AudioProcessor::prepare(int blockSize)
{
    for (auto* processor : trackProcessors)
    {
        processor->prepare(blockSize);
    }
}


bool AudioProcessor::mixEnded()
{
    TrackProcessor* leader = nullptr;
    TrackProcessor* follower = nullptr;
    
    getTrackProcessors(&leader, &follower);
    
    if (leader)
    {
        if (leader->mixEnded())
            return true;
    }
    
    return false;
}


void AudioProcessor::reset()
{
    paused.store(true);
    skipFlag.store(false);
    
    for (auto* processor : trackProcessors)
        processor->reset();
}


void AudioProcessor::skipToNextEvent()
{
    TrackProcessor* leader = nullptr;
    TrackProcessor* follower = nullptr;
    
    getTrackProcessors(&leader, &follower);
    
    if (leader)
    {
        leader->skipToNextEvent();
        skipFlag.store(false);
    }
}
