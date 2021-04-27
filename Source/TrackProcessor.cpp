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
    
    shifter.setSampleRate(SUPPORTED_SAMPLERATE);
    shifter.setChannels(1);
    shifter.clear();
}


int TrackProcessor::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (!ready.load()) return false;
    
    const juce::ScopedLock sl(lock);
    
    if (play)
    {
        if (output.getNumSamples() != bufferToFill.numSamples) jassert(false);
            
        processShifts(bufferToFill.numSamples);
//        simpleCopy(bufferToFill.numSamples);
        
        output.applyGain(std::sqrt(track->gain.currentValue));
        
        bufferToFill.buffer->addFrom(0, bufferToFill.startSample, output.getReadPointer(0), bufferToFill.numSamples);
        bufferToFill.buffer->addFrom(1, bufferToFill.startSample, output.getReadPointer(0), bufferToFill.numSamples);
        
        if (trackEnd)
            loadNextTrack();
    }
    
    return track->getPlayhead();
}


Track* TrackProcessor::getTrack()
{
    if (ready.load())
        return track.get();
    else
        return nullptr;
}


void TrackProcessor::nextMix()
{
    currentMix = dj->getNextMix(currentMix);
    track->applyNextMix(&currentMix);
}


void TrackProcessor::loadNextTrack()
{
    ready.store(false);
    shifter.clear();
    
    currentMix = dj->getNextMix(currentMix);
    track->applyNextMix(&currentMix);
    
    play = false;
    trackEnd = false;
    newTrack = true;
    
    partner->nextMix();
    
    ready.store(true);
}


void TrackProcessor::loadFirstTrack(TrackInfo trackInfo, bool leader)
{
    ready.store(false);
    
    currentMix = dj->getNextMix(currentMix);
    
    if (leader)
    {
        track->info = trackInfo;
        track->bpm.moveTo(trackInfo.bpm);
        track->gain.moveTo(1.0);
        track->audio = dataManager->loadAudio(track->info.filename, true);
        resetPlayhead();
        track->applyNextMix(&currentMix);
        play = true;
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
    }
}


void TrackProcessor::update(int numSamples)
{
    trackEnd = track->update(numSamples);
    
    // Update time & pitch shifts
    timeStretch = double(track->bpm.currentValue) / track->info.bpm;
    shifter.setTempo(timeStretch);
    shifter.setPitchSemiTones(track->pitch.currentValue);
}


void TrackProcessor::resetPlayhead(int sample)
{
    track->resetPlayhead(sample);
    shifterPlayhead = sample;
}


void TrackProcessor::processShifts(int numSamples)
{
    double ratio = shifter.getInputOutputSampleRatio();
    int numInput = round(double(numSamples) / ratio);
    
    while (shifter.numSamples() < numSamples)
    {
        if (shifterPlayhead >= getAudioLength() - numInput)
            return; // TODO: handle this better
        
        shifter.putSamples(track->audio->getReadPointer(0, shifterPlayhead), numInput);
        shifterPlayhead += numInput;
    }
    
    shifter.receiveSamples(output.getWritePointer(0), numSamples);
    
    update(numInput);
}


void TrackProcessor::simpleCopy(int numSamples)
{
    if (shifterPlayhead >= getAudioLength() - numSamples) return;
    
    output.copyFrom(0, 0, *track->audio, 0, shifterPlayhead, numSamples);
    shifterPlayhead += numSamples;
    update(numSamples);
}
