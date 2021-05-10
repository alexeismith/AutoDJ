//
//  TrackProcessor.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 19/04/2021.
//

#include "TrackProcessor.hpp"

#include "CommonDefs.hpp"

#include "ArtificialDJ.hpp"


TrackProcessor::TrackProcessor(TrackDataManager* dm, ArtificialDJ* DJ) :
    dataManager(dm), dj(DJ)
{
    trackLoader.reset(new TrackLoadThread(this));
    
    ready.store(false);
    
    track.reset(new Track());
    
    stretcher.reset(new TimeStretcher());
}


int TrackProcessor::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (!ready.load()) return false;
    
    const juce::ScopedLock sl(lock);
    
    if (play)
    {
        if (output.getNumSamples() != bufferToFill.numSamples) jassert(false);
        
        int numProcessed = stretcher->process(track->audio, &output, bufferToFill.numSamples);
        update(numProcessed);
        
        output.applyGain(std::sqrt(track->gain.currentValue));
        
        bufferToFill.buffer->addFrom(0, bufferToFill.startSample, output.getReadPointer(0), bufferToFill.numSamples);
        bufferToFill.buffer->addFrom(1, bufferToFill.startSample, output.getReadPointer(0), bufferToFill.numSamples);
        
        if (trackEnd)
            loadNextTrack();
    }
    
    return track->getPlayhead();
}


void TrackProcessor::nextMix()
{
    currentMix = dj->getNextMix(currentMix);
    track->applyNextMix(&currentMix);
}


void TrackProcessor::loadNextTrack()
{
    ready.store(false);
    
    stretcher->reset();
    
    track->info->playing = false;
    track->info->played = true;
    dataManager->trackDataUpdate.store(true);
    
    currentMix = dj->getNextMix(currentMix);
    if (track->applyNextMix(&currentMix))
    {
        play = false;
        trackEnd = false;
        newTrack = true;
        
        ready.store(true);
    }
    
    partner->nextMix();
}


void TrackProcessor::loadFirstTrack(TrackInfo* trackInfo, bool leader, juce::AudioBuffer<float>* audio)
{
    ready.store(false);
    
    currentMix = dj->getNextMix(currentMix);
    
    if (leader)
    {
        track->info = trackInfo;
        track->audio = audio;
        track->bpm.moveTo(trackInfo->bpm);
        track->gain.moveTo(1.0);
        resetPlayhead();
        track->applyNextMix(&currentMix);
        play = true;
        
        track->info->playing = true;
        dataManager->trackDataUpdate.store(true);
    }
    else
    {
        track->leader = true;
        track->applyNextMix(&currentMix);
    }
    
    newTrack = true;
    
    ready.store(true);
}


void TrackProcessor::prepare(int blockSize)
{
    output.setSize(1, blockSize);
    stretcher->prepare(blockSize);
}


Track* TrackProcessor::getNewTrack()
{
    if (newTrack)
    {
        newTrack = false;
        return getTrack();
    }
    
    return nullptr;
}


void TrackProcessor::syncWithLeader(int leaderPlayhead)
{
    if (!play && leaderPlayhead >= currentMix.start)
    {
        resetPlayhead(currentMix.startNext + (leaderPlayhead - currentMix.start));
        play = true;
        track->info->playing = true;
        dataManager->trackDataUpdate.store(true);
    }
}


void TrackProcessor::skipToNextEvent()
{
    jassert(isLeader()); // Must invoke this call on the leader only!
    
    int playhead = track->getPlayhead();
    MixInfo* mix = track->getCurrentMix();
    
    if (playhead < mix->start)
    {
        playhead = mix->start;
        track->bpm.resetTo(mix->bpm);
    }
    else if (playhead < mix->end)
    {
        playhead = mix->end;
        partner->resetPlayhead(mix->endNext);
    }
    
    resetPlayhead(playhead);
}


void TrackProcessor::update(int numSamples)
{
    trackEnd = track->update(numSamples);
    
    // Update time shift
    stretcher->update(track->info->bpm, track->bpm.currentValue);
}


void TrackProcessor::resetPlayhead(int sample)
{
    track->resetPlayhead(sample);
    stretcher->resetPlayhead(sample);
}
