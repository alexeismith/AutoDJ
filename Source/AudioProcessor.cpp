//
//  AudioProcessor.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 09/04/2021.
//

#include "AudioProcessor.hpp"

#include "CommonDefs.hpp"


AudioProcessor::AudioProcessor(TrackDataManager* dataManager, ArtificialDJ* dj)
{
    paused.store(true);
    
    trackProcessors.add(new TrackProcessor(dataManager, dj));
    trackProcessors.add(new TrackProcessor(dataManager, dj));
    
    previewProcessor.reset(new TrackProcessor(dataManager, dj));
}


void AudioProcessor::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    TrackProcessor* leader = nullptr;
    TrackProcessor* next = nullptr;
    
    bufferToFill.clearActiveBufferRegion();
    
    if (paused.load())
    {
        previewProcessor->getNextAudioBlock(bufferToFill);
    }
    else
    {
        getProcessors(&leader, &next);
        
        if (!leader)
            DBG("NO LEADER");
        if (!next)
            DBG("NO NEXT");
        
        if (leader)
        {
            bool play = leader->getNextAudioBlock(bufferToFill);
            next->getNextAudioBlock(bufferToFill, play);
        }
    }
}


void AudioProcessor::play(TrackData track)
{
//    trackProcessors.getFirst()->load(track);
    play();
}


void AudioProcessor::preview(TrackData track, int startSample, int numSamples)
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
