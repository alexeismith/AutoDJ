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
        
        if (filterOn)
        {
            highPassFilterL.processSamples(output.getWritePointer(0), bufferToFill.numSamples);
            highPassFilterR.processSamples(output.getWritePointer(1), bufferToFill.numSamples);
        }
        
        bufferToFill.buffer->addFrom(0, bufferToFill.startSample, output.getReadPointer(0), bufferToFill.numSamples);
        bufferToFill.buffer->addFrom(1, bufferToFill.startSample, output.getReadPointer(1), bufferToFill.numSamples);
        
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


bool TrackProcessor::isReady(bool& end)
{
    end = mixEnd.load();
    return ready.load();
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
    
    newTrack = true;
    
    ready.store(true);
}


void TrackProcessor::prepare(int blockSize)
{
    output.setSize(2, blockSize);
    stretcher->prepare(blockSize);
}


Track* TrackProcessor::getNewTrack()
{
    if (newTrack.load())
    {
        newTrack.store(false);
        return getTrack();
    }
    
    return nullptr;
}


void TrackProcessor::cue(int leaderPlayhead)
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


void TrackProcessor::reset()
{
    play = false;
    trackEnd = false;
    
    newTrack.store(true);
    mixEnd.store(false);

    stretcher->reset();
    output.clear();
    
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
