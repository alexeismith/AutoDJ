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
    for (int i = 0; i < NUM_CONCURRENT_TRACKS; i++)
    {
        trackProcessors.add(new TrackProcessor(dataManager));
    }
}


void AudioProcessor::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    
    for (auto* trackProcessor : trackProcessors)
        trackProcessor->getNextAudioBlock(bufferToFill);
}


void AudioProcessor::play(TrackData track)
{
    trackProcessors.getFirst()->load(track);
}
