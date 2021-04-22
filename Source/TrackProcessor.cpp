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
            processShifts(bufferToFill);
            bufferToFill.buffer->addFrom(1, bufferToFill.startSample, bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample), bufferToFill.numSamples);
            updateState();
        }
        
        return inputPlayhead >= state->currentMix->start;
    }
    
    return false;
}


void TrackProcessor::loadTrack()
{
    reset();
    
    dataManager->fetchAudio(state->track.filename, input, true); // TODO: change to stereo
    inputLength = input.getNumSamples();
    inputPlayhead = state->currentMix->startNext;
    
    ready = true;
}


void TrackProcessor::seekClip(int sample, int length)
{
    seek(sample);
    inputLength = juce::jmin(sample+length, input.getNumSamples());
}


void TrackProcessor::updateState()
{
    if (state->update(inputPlayhead))
        loadTrack();
    
    // This processor needs to know when to start playing based on other processor - global clock?
    
    shifter.setTempo(double(state->bpm.currentValue) / state->track.bpm);
    shifter.setPitchSemiTones(state->pitch.currentValue);
}


void TrackProcessor::reset()
{
    ready = false;
    
    shifter.clear();
}


void TrackProcessor::processShifts(const juce::AudioSourceChannelInfo& bufferToFill)
{
    double ratio = shifter.getInputOutputSampleRatio();
    int numInput = round(double(bufferToFill.numSamples) / ratio);
    
    while (shifter.numSamples() < bufferToFill.numSamples)
    {
        if (inputPlayhead >= inputLength - numInput)
        {
            inputPlayhead = -1;
            return;
        }
        
        shifter.putSamples(input.getReadPointer(0, inputPlayhead), numInput);
        inputPlayhead += numInput;
    }
    
    // TODO: don't overwrite bufferToFill
    shifter.receiveSamples(bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample), bufferToFill.numSamples);
}
