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


void TrackProcessor::getNextAudioBlock(const juce::AudioSourceChannelInfo& outputBuffer)
{
    // If a track isn't loaded, return
    if (!ready.load()) return;
    
    const juce::ScopedLock sl(lock);
    
    // If this track should not be playing yet, return
    if (!play)
        return;

    // Throw a debug error if the intermediate audio buffer is not the same size as the output
    // We perform processing for this track in an intermediate buffer,
    // so that any samples already in the output buffer are not disurbed
    if (processBuffer.getNumSamples() != outputBuffer.numSamples) jassert(false);
    
    // The number of output samples required is outputBuffer.numSamples
    // We need to fetch that many samples of the track POST-stretch
    // This function gets the stretched samples and return the number of input samples that were processed (PRE-stretch)
    // See TimeStretcher for more info on the time-strech factor and its effects
    int numProcessed = stretcher->process(track->audio, &processBuffer, outputBuffer.numSamples);
    
    // Update the track parameters based on how many samples were processed
    // The timing of mix parameters in MixInfo is measured in terms of the original track audio,
    // hence we update by the number of PRE-stretch samples that were processed, to progress the parameters according to the original audio
    update(numProcessed);
    
    // Apply any crossfade gain
    processBuffer.applyGain(std::sqrt(track->gain.currentValue));
    
    // If the high-pass crossfade filtering is active, apply the left and right channel filters
    if (filterOn)
    {
        highPassFilterL.processSamples(processBuffer.getWritePointer(0), outputBuffer.numSamples);
        highPassFilterR.processSamples(processBuffer.getWritePointer(1), outputBuffer.numSamples);
    }
    
    // Add the processed audio from the intermediate buffer into the output
    outputBuffer.buffer->addFrom(0, outputBuffer.startSample, processBuffer.getReadPointer(0), outputBuffer.numSamples);
    outputBuffer.buffer->addFrom(1, outputBuffer.startSample, processBuffer.getReadPointer(1), outputBuffer.numSamples);
    
    // If the track has ended, load the next one
    if (trackEnd)
        loadNextTrack();
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
    // If this processor is not already playing, and the leading processor
    // has reached the point where the transition should start...
    if (!play && leaderPlayhead >= currentMix.leaderStart)
    {
        // Set the playhead to the start point
        resetPlayhead(currentMix.followerStart + (leaderPlayhead - currentMix.leaderStart));
        // Start this track
        play = true;
        // Update the track info so it shows as playing in the Library and Direction views
        track->info->playing = true;
        dataManager->trackDataUpdate.store(true);
    }
}


void TrackProcessor::skipToNextEvent()
{
    jassert(isLeader()); // Must invoke this call on the leader only!
    
    // Fetch the track playhead and current mix
    int playhead = track->getPlayhead();
    MixInfo* mix = track->getCurrentMix();
    
    // If we have not yet reached the next transition
    if (playhead < mix->leaderStart)
    {
        // Skip to the next transition
        playhead = mix->leaderStart;
        track->bpm.resetTo(mix->bpm);
    }
    // Otherwise, if we are mid-transition
    else if (playhead < mix->leaderEnd)
    {
        // Skip to the end of the transition
        playhead = mix->leaderEnd;
        partner->resetPlayhead(mix->followerEnd);
    }
    
    // Apply the playhead change
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
    // If the supplied frequency is 0, just turn off filtering and reset
    if (frequency <= 0)
    {
        filterOn = false;
        highPassFilterL.reset();
        highPassFilterR.reset();
        return;
    }
    // Otherwise, filtering is active so apply the filters of the left and right channels...
    
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
