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
    ready.store(false);
    
    shifter.setSampleRate(SUPPORTED_SAMPLERATE);
    shifter.setChannels(1);
    shifter.clear();
//    shifter.setPitchSemiTones(2);
//    shifter.setRate(1.2);
    shifter.setTempoChange(10);
    DBG("SoundTouch initial latency: " << shifter.getSetting(SETTING_INITIAL_LATENCY));
    DBG("Nominal input: " << shifter.getSetting(SETTING_NOMINAL_INPUT_SEQUENCE));
    DBG("Nominal output: " << shifter.getSetting(SETTING_NOMINAL_OUTPUT_SEQUENCE));
}


void TrackProcessor::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (!ready.load()) return;

    if (inputPlayhead > -1)
    {
        processShift(bufferToFill);
        bufferToFill.buffer->addFrom(1, bufferToFill.startSample, bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample), bufferToFill.numSamples);
    }
}


void TrackProcessor::load(TrackData track)
{
    reset();
    
    currentTrack = track;
    dataManager->fetchAudio(track.filename, input, true); // TODO: change to stereo
    inputPlayhead = 0;
    
    ready.store(true);
}


void TrackProcessor::reset()
{
    ready.store(false);
    
    inputPlayhead = -1;
    
    shifter.clear();
}


void TrackProcessor::processShift(const juce::AudioSourceChannelInfo& bufferToFill)
{
    double ratio = shifter.getInputOutputSampleRatio();
    int numInput = round(double(bufferToFill.numSamples) / ratio);
    
    while (shifter.numSamples() < bufferToFill.numSamples)
    {
        if (inputPlayhead >= input.getNumSamples() - numInput)
        {
            inputPlayhead = -1;
            return;
        }
        
        shifter.putSamples(input.getReadPointer(0, inputPlayhead), numInput);
        inputPlayhead += numInput;
    }
    
    shifter.receiveSamples(bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample), bufferToFill.numSamples);
}
