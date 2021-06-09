//
//  TrackProcessor.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 19/04/2021.
//

#include "TrackProcessor.hpp"

#include "CommonDefs.hpp"

#include "ArtificialDJ.hpp"


TrackProcessor::TrackProcessor(DataManager* dm, ArtificialDJ* DJ) :
    dataManager(dm), dj(DJ)
{
    ready.store(false);
    
    track.reset(new Track());
    
    stretcher.reset(new TimeStretcher());
}


int TrackProcessor::getNextAudioBlock(const juce::AudioSourceChannelInfo& outputBuffer)
{
    if (!ready.load()) return false;
    
    const juce::ScopedLock sl(lock);
    
    if (play)
    {
        if (processBuffer.getNumSamples() != outputBuffer.numSamples) jassert(false);
        
        int numProcessed = stretcher->process(track->audio, &processBuffer, outputBuffer.numSamples);
        
        update(numProcessed);
        
        processBuffer.applyGain(std::sqrt(track->gain.currentValue));
        
        if (filterOn)
        {
            highPassFilterL.processSamples(processBuffer.getWritePointer(0), outputBuffer.numSamples);
            highPassFilterR.processSamples(processBuffer.getWritePointer(1), outputBuffer.numSamples);
        }
        
        outputBuffer.buffer->addFrom(0, outputBuffer.startSample, processBuffer.getReadPointer(0), outputBuffer.numSamples);
        outputBuffer.buffer->addFrom(1, outputBuffer.startSample, processBuffer.getReadPointer(1), outputBuffer.numSamples);
        
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
        
        ready.store(true);
    }
    else // Mix is ending
    {
        mixEnd.store(true);
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
    
    ready.store(true);
}


void TrackProcessor::prepare(int blockSize)
{
    processBuffer.setSize(2, blockSize);
    stretcher->prepare(blockSize);
}


void TrackProcessor::cue(int leaderPlayhead)
{
    if (!play && leaderPlayhead >= currentMix.leaderStart)
    {
        resetPlayhead(currentMix.followerStart + (leaderPlayhead - currentMix.leaderStart));
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
    
    if (playhead < mix->leaderStart)
    {
        playhead = mix->leaderStart;
        track->bpm.resetTo(mix->bpm);
    }
    else if (playhead < mix->leaderEnd)
    {
        playhead = mix->leaderEnd;
        partner->resetPlayhead(mix->followerEnd);
    }
    
    resetPlayhead(playhead);
}


void TrackProcessor::reset()
{
    play = false;
    trackEnd = false;
    
    mixEnd.store(false);

    stretcher->reset();
    processBuffer.clear();
    
    track.reset(new Track());
    
    highPassFilterL.reset();
    highPassFilterR.reset();
}


void TrackProcessor::setHighPass(int frequency)
{
    if (frequency <= 0)
    {
        filterOn = false;
        highPassFilterL.reset();
        highPassFilterR.reset();
        return;
    }
    
    filterOn = true;
    
    highPassFilterL.setCoefficients(juce::IIRCoefficients::makeHighPass(SUPPORTED_SAMPLERATE, frequency, 1.0));
    highPassFilterR.setCoefficients(juce::IIRCoefficients::makeHighPass(SUPPORTED_SAMPLERATE, frequency, 1.0));
}


void TrackProcessor::update(int numSamples)
{
    trackEnd = track->update(numSamples);
    
    setHighPass(round(track->highPassFreq.currentValue));
    
    // Update time shift
    stretcher->update(track->info->bpm, track->bpm.currentValue);
}


void TrackProcessor::resetPlayhead(int sample)
{
    track->resetPlayhead(sample);
    stretcher->resetPlayhead(sample);
}
