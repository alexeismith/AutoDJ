//
//  AudioProcessor.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 09/04/2021.
//

#include "AudioProcessor.hpp"

#include "CommonDefs.hpp"


AudioProcessor::AudioProcessor(TrackDataManager* dataManager, ArtificialDJ* dj, int initBlockSize)
{
    trackProcessors.add(new TrackProcessor(dataManager, dj));
    trackProcessors.add(new TrackProcessor(dataManager, dj));
    
    getTrackProcessor(0)->setPartner(getTrackProcessor(1));
    getTrackProcessor(1)->setPartner(getTrackProcessor(0));
    
    prepare(initBlockSize);
}


void AudioProcessor::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    TrackProcessor* leader = nullptr;
    TrackProcessor* next = nullptr;
    
    bufferToFill.clearActiveBufferRegion();
    
    if (skipFlag.load())
        skipToNextEvent();
    
    if (!paused.load())
    {
        getTrackProcessors(&leader, &next);
        
        if (leader)
        {
            int playhead = leader->getNextAudioBlock(bufferToFill);
            next->getNextAudioBlock(bufferToFill);
            next->cue(playhead);
        }
        else
        {
            jassert(false); // No leader!
        }
    }
    
    if (volume != targetVolume.load())
    {
        bufferToFill.buffer->applyGainRamp(bufferToFill.startSample, bufferToFill.numSamples, volume, targetVolume.load());
        volume = targetVolume.load();
    }
    else
    {
        bufferToFill.buffer->applyGain(bufferToFill.startSample, bufferToFill.numSamples, volume);
    }
}


void AudioProcessor::getTrackProcessors(TrackProcessor** leader, TrackProcessor** next)
{
    for (auto* processor : trackProcessors)
    {
        if (processor->isLeader())
            *leader = processor;
        else
            *next = processor;
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
    TrackProcessor* next = nullptr;
    
    getTrackProcessors(&leader, &next);
    
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
    TrackProcessor* next = nullptr;
    
    getTrackProcessors(&leader, &next);
    
    if (leader)
    {
        leader->skipToNextEvent();
        skipFlag.store(false);
    }
}
