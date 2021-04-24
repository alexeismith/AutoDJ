//
//  TrackProcessor.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 19/04/2021.
//

#include "TrackProcessor.hpp"

#include "CommonDefs.hpp"


TrackProcessor::TrackProcessor(TrackDataManager* dm, ArtificialDJ* dj) :
    dataManager(dm)
{
    ready = false;
    
    state.reset(new TrackState(TrackData(), dj, nullptr, 0.0, 0.0, 0.0));
    
    shifter.setSampleRate(SUPPORTED_SAMPLERATE);
    shifter.setChannels(1);
    shifter.clear();
//    shifter.setPitchSemiTones(2);
//    shifter.setRate(1.2);
//    shifter.setTempoChange(10);
    DBG("SoundTouch initial latency: " << shifter.getSetting(SETTING_INITIAL_LATENCY));
    DBG("Nominal input: " << shifter.getSetting(SETTING_NOMINAL_INPUT_SEQUENCE));
    DBG("Nominal output: " << shifter.getSetting(SETTING_NOMINAL_OUTPUT_SEQUENCE));
}


bool TrackProcessor::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill, bool play)
{
    const juce::ScopedLock sl (lock);
    
    if (ready)
    {
        if (play)
        {
            if (output.getNumSamples() != bufferToFill.numSamples) jassert(false);
            
            if(isLeader())
                DBG("Leader: " << playhead << " gain: " << state->gain.currentValue);
            else
                DBG("Next: " << playhead << " gain: " << state->gain.currentValue);
                
            processShifts(bufferToFill.numSamples);
//            simpleCopy(bufferToFill.numSamples);
            
            output.applyGain(std::sqrt(state->gain.currentValue));
            
            if(isLeader())
                bufferToFill.buffer->addFrom(0, bufferToFill.startSample, output.getReadPointer(0), bufferToFill.numSamples);
            else
                bufferToFill.buffer->addFrom(1, bufferToFill.startSample, output.getReadPointer(0), bufferToFill.numSamples);
            
            updateState();
        }
        
        return playhead >= state->currentMix->start;
    }
    
    return false;
}


void TrackProcessor::loadTrack()
{
    ready = false;
    shifter.clear();
    
    DBG("loadTrack");
    
    dataManager->fetchAudio(state->track.filename, input, true); // TODO: change to stereo
    inputLength = input.getNumSamples();
    playhead = state->currentMix->startNext;
    inputPlayhead = playhead;
    
    ready = true;
}


void TrackProcessor::seekClip(int sample, int length)
{
    seek(sample);
    inputLength = juce::jmin(sample+length, input.getNumSamples());
}


void TrackProcessor::updateState()
{
//    if (state->leader)
//        DBG("Leader: " << inputPlayhead);
//    else
//        DBG("Next: " << inputPlayhead);
    
    if (state->update(playhead))
        loadTrack();
    
    // This processor needs to know when to start playing based on other processor - global clock?
    
    shifter.setTempo(double(state->bpm.currentValue) / state->track.bpm);
    shifter.setPitchSemiTones(state->pitch.currentValue);
}


void TrackProcessor::initialise(TrackData track)
{
    ready = false;
    shifter.clear();
    
    state->track = track;
    state->bpm.moveTo(track.bpm);
    state->gain.moveTo(1.0);
    state->currentSample = 0;
    dataManager->fetchAudio(state->track.filename, input, true); // TODO: change to stereo
    inputLength = input.getNumSamples();
    
    playhead = 0;
    inputPlayhead = 0;
    
    state->applyNextMix();
    
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
        if (inputPlayhead >= inputLength - numInput)
            return; // TODO: handle this better
        
        shifter.putSamples(input.getReadPointer(0, inputPlayhead), numInput);
        inputPlayhead += numInput;
    }
    
    playhead += numInput;
    
    shifter.receiveSamples(output.getWritePointer(0), numSamples);
}


void TrackProcessor::simpleCopy(int numSamples)
{
    if (inputPlayhead >= inputLength - numSamples) return;
    
    output.copyFrom(0, 0, input, 0, inputPlayhead, numSamples);
    inputPlayhead += numSamples;
    playhead += numSamples;
}
