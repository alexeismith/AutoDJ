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
    shifter.setPitchSemiTones(1);
    DBG("SoundTouch initial latency: " << shifter.getSetting(SETTING_INITIAL_LATENCY));
    DBG("Nominal input: " << shifter.getSetting(SETTING_NOMINAL_INPUT_SEQUENCE));
    DBG("Nominal output: " << shifter.getSetting(SETTING_NOMINAL_OUTPUT_SEQUENCE));
}


void TrackProcessor::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (!ready.load()) return;
    
    int numSamples = bufferToFill.numSamples;

    if (inputPlayhead > -1 && inputPlayhead < (input.getNumSamples() - numSamples))
    {
        memcpy(bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample), input.getReadPointer(0, inputPlayhead), numSamples*sizeof(float));
        memcpy(bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample), input.getReadPointer(0, inputPlayhead), numSamples*sizeof(float));

        inputPlayhead += numSamples;
    }
    else
    {
        inputPlayhead = -1;
    }
}


void TrackProcessor::load(TrackData track)
{
    ready.store(false);
    
    currentTrack = track;
    inputPlayhead = 0;
    outputPlayhead = 0;
    
    dataManager->fetchAudio(track.filename, input);
    
    ready.store(true);
}


void TrackProcessor::processShift()
{
}
