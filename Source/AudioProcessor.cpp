//
//  AudioProcessor.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 09/04/2021.
//

#include "AudioProcessor.hpp"

#include "CommonDefs.hpp"


AudioProcessor::AudioProcessor(TrackDataManager* dataManager)
{
    paused.store(true);
    
    for (int i = 0; i < NUM_CONCURRENT_TRACKS; i++)
    {
        trackProcessors.add(new TrackProcessor(dataManager));
    }
    
    previewProcessor.reset(new TrackProcessor(dataManager));
}


void AudioProcessor::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    
    if (paused.load())
    {
        previewProcessor->getNextAudioBlock(bufferToFill);
    }
    else
    {
        for (auto* trackProcessor : trackProcessors)
            trackProcessor->getNextAudioBlock(bufferToFill);
    }
}


void AudioProcessor::play(TrackData track)
{
    trackProcessors.getFirst()->load(track);
    play();
}


void AudioProcessor::preview(TrackData track, int startSample, int numSamples)
{
    previewProcessor->load(track);
    previewProcessor->seekClip(startSample, numSamples);
    
    paused.store(true);
}
