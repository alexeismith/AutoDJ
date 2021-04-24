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
    
    getProcessor(0)->setPartner(getProcessor(1));
    getProcessor(1)->setPartner(getProcessor(0));
    
    prepare(initBlockSize);
}


void AudioProcessor::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    TrackProcessor* leader = nullptr;
    TrackProcessor* next = nullptr;
    
    bufferToFill.clearActiveBufferRegion();
    
    if (paused.load())
    {
        // Preview
    }
    else
    {
        getProcessors(&leader, &next);
        
        if (leader)
        {
            bool play = leader->getNextAudioBlock(bufferToFill);
            next->getNextAudioBlock(bufferToFill, play);
        }
        else
        {
            jassert(false); // No leader!
        }
    }
}


void AudioProcessor::play(TrackInfo track)
{
//    trackProcessors.getFirst()->load(track);
    play();
}


void AudioProcessor::preview(TrackInfo track, int startSample, int numSamples)
{
//    previewProcessor->load(track);
//    previewProcessor->seekClip(startSample, numSamples);
    
    paused.store(true);
}


void AudioProcessor::getProcessors(TrackProcessor** leader, TrackProcessor** next)
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
