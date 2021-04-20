//
//  TrackProcessor.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 19/04/2021.
//

#include "TrackProcessor.hpp"

#include "CommonDefs.hpp"


TrackProcessor::TrackProcessor(TrackDataManager* dm) :
    dataManager(dm)
{
    ready = false;
    
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


void TrackProcessor::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    const juce::ScopedLock sl (lock);
    
    if (!ready) return;

    if (inputPlayhead > -1)
    {
        processShifts(bufferToFill);
        bufferToFill.buffer->addFrom(1, bufferToFill.startSample, bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample), bufferToFill.numSamples);
    }
}


void TrackProcessor::load(TrackData track)
{
    reset();
    
    currentTrack = track;
    dataManager->fetchAudio(track.filename, input, true); // TODO: change to stereo
    inputLength = input.getNumSamples();
    inputPlayhead = 0;
    
    ready = true;
}


void TrackProcessor::seekClip(int sample, int length)
{
    seek(sample);
    inputLength = juce::jmin(sample+length, input.getNumSamples());
}


void TrackProcessor::reset()
{
    ready = false;
    
    inputPlayhead = -1;
    
    shiftBpmCurrent = 0;
    shiftPitchCurrent = 0;
    shiftBpmSamplesRemaining = -1;
    shiftPitchSamplesRemaining = -1;
    
    shifter.clear();
}


void TrackProcessor::processShifts(const juce::AudioSourceChannelInfo& bufferToFill)
{
    int inputBufferCounter = 0;
    
    double ratio = shifter.getInputOutputSampleRatio();
    int numInput = round(double(bufferToFill.numSamples) / ratio);
    
    while (shifter.numSamples() < bufferToFill.numSamples)
    {
        if (inputPlayhead >= inputLength - numInput)
        {
            inputPlayhead = -1;
            return;
        }
        
        inputBufferCounter += 1;
        
        shifter.putSamples(input.getReadPointer(0, inputPlayhead), numInput);
        inputPlayhead += numInput;
    }
    
    shifter.receiveSamples(bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample), bufferToFill.numSamples);
    
    updateShifts(numInput * inputBufferCounter);
}


void TrackProcessor::newTempoShift(double shiftBpm, int byInputSample)
{
    newShift(shiftBpm, byInputSample, shiftBpmCurrent, shiftBpmTarget, shiftBpmRate, shiftBpmSamplesRemaining);
}

void TrackProcessor::newPitchShift(double shiftSemitones, int byInputSample)
{
    newShift(shiftSemitones, byInputSample, shiftPitchCurrent, shiftPitchTarget, shiftPitchRate, shiftPitchSamplesRemaining);
}

void TrackProcessor::newShift(double shift, int byInputSample, double& current, double& target, double& rate, int& samplesRemaining)
{
    if (byInputSample == -1)
    {
        samplesRemaining = -1;
        current = shift;
    }
    else
    {
        target = shift;
        samplesRemaining = byInputSample - inputPlayhead;
        rate = (target - current) / samplesRemaining;
    }
}

void TrackProcessor::updateShifts(int samplesRequested)
{
    updateShift(shiftBpmCurrent, shiftBpmTarget, shiftBpmRate, shiftBpmSamplesRemaining, samplesRequested);
    updateShift(shiftPitchCurrent, shiftPitchTarget, shiftPitchRate, shiftPitchSamplesRemaining, samplesRequested);
    
    shifter.setTempo(double(shiftBpmCurrent + currentTrack.bpm) / currentTrack.bpm);
    shifter.setPitchSemiTones(shiftPitchCurrent);
}


void TrackProcessor::updateShift(double& current, double target, double rate, int& samplesRemaining, int samplesRequested)
{
    if (samplesRemaining < 0) return;
    
    samplesRemaining -= samplesRequested;
    
    if (samplesRemaining <= 0)
    {
        current = target;
        samplesRemaining = -1;
    }
    else
    {
        current += rate * samplesRequested;
    }
}
