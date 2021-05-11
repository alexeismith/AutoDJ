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
    paused.store(true);
    
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
    
    if (paused.load())
    {
        // Preview
    }
    else
    {
        getTrackProcessors(&leader, &next);
        
        if (leader)
        {
            int playhead = leader->getNextAudioBlock(bufferToFill);
            next->syncWithLeader(playhead - bufferToFill.numSamples);
            next->getNextAudioBlock(bufferToFill);
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


void AudioProcessor::play(TrackInfo track)
{
    play();
}


void AudioProcessor::preview(TrackInfo track, int startSample, int numSamples)
{
//    previewProcessor->load(track);
//    previewProcessor->seekClip(startSample, numSamples);
    
    paused.store(true);
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
