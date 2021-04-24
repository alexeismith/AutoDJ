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
    ready = false;
    
    track.reset(new Track());
    
    shifter.setSampleRate(SUPPORTED_SAMPLERATE);
    shifter.setChannels(1);
    shifter.clear();
}


bool TrackProcessor::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill, bool play)
{
    const juce::ScopedLock sl(lock);
    
    if (ready)
    {
        if (play)
        {
            if (output.getNumSamples() != bufferToFill.numSamples) jassert(false);
            
            if(isLeader())
                DBG("Leader: " << track->playhead << " gain: " << track->gain.currentValue);
            else
                DBG("Next: " << track->playhead << " gain: " << track->gain.currentValue);
                
            processShifts(bufferToFill.numSamples);
//            simpleCopy(bufferToFill.numSamples);
            
            output.applyGain(std::sqrt(track->gain.currentValue));
            
            if(isLeader())
                bufferToFill.buffer->addFrom(0, bufferToFill.startSample, output.getReadPointer(0), bufferToFill.numSamples);
            else
                bufferToFill.buffer->addFrom(1, bufferToFill.startSample, output.getReadPointer(0), bufferToFill.numSamples);
            
            update();
        }
        
        return track->playhead >= track->currentMix->start;
    }
    
    return false;
}


void TrackProcessor::update()
{
    if (track->update())
        loadNextTrack();
    
    shifter.setTempo(double(track->bpm.currentValue) / track->info.bpm);
    shifter.setPitchSemiTones(track->pitch.currentValue);
}


void TrackProcessor::loadNextTrack()
{
    ready = false;
    shifter.clear();
    
    DBG("loadTrack");
    
    MixInfo* newMix = dj->getNextMix(track->currentMix);
    track->applyNextMix(newMix);
    track->audio = dataManager->loadAudio(track->info.filename, true); // TODO: change to stereo
    track->playhead = track->currentMix->startNext;
    shifterPlayhead = track->playhead;
    
    ready = true;
}


void TrackProcessor::loadFirstTrack(TrackInfo trackInfo)
{
    ready = false;
    
    track->info = trackInfo;
    track->bpm.moveTo(trackInfo.bpm);
    track->gain.moveTo(1.0);
    track->playhead = 0;
    track->audio = dataManager->loadAudio(track->info.filename, true); // TODO: change to stereo
    track->applyNextMix(dj->getNextMix(nullptr));
    
    shifterPlayhead = 0;
    shifter.setTempo(1.0);
    shifter.setPitchSemiTones(0.0);
    
    ready = true;
}


void TrackProcessor::prepare(int blockSize)
{
    output.setSize(1, blockSize);
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
    
    track->playhead += numInput;
    
    shifter.receiveSamples(output.getWritePointer(0), numSamples);
}


void TrackProcessor::simpleCopy(int numSamples)
{
    if (shifterPlayhead >= getAudioLength() - numSamples) return;
    
    output.copyFrom(0, 0, *track->audio, 0, shifterPlayhead, numSamples);
    shifterPlayhead += numSamples;
    track->playhead += numSamples;
}