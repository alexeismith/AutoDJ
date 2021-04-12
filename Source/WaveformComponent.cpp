//
//  WaveformComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 08/04/2021.
//

#include "WaveformComponent.hpp"

#include "CommonDefs.hpp"


WaveformComponent::WaveformComponent(int width)
{
    setSize(width, 300);
    
    filterLow.setCoefficients(juce::IIRCoefficients::makeLowPass(SUPPORTED_SAMPLERATE, 200, 1.0));
    filterMid.setCoefficients(juce::IIRCoefficients::makeBandPass(SUPPORTED_SAMPLERATE, 500, 1.0));
    filterHigh.setCoefficients(juce::IIRCoefficients::makeHighPass(SUPPORTED_SAMPLERATE, 10000, 1.0));
    
    processBuffers.setSize(4, WAVEFORM_FRAME_SIZE);
}


void WaveformComponent::paint(juce::Graphics& g)
{
    float magnitude;
    
    g.setColour(juce::Colours::black);
    g.fillAll();
    
    for (int x = 0; x < getWidth(); x++)
    {
        magnitude = levels[x] * getHeight() * 0.5f;
        
        g.setColour(colours[x]);
        g.drawVerticalLine(x, 0.5f * getHeight() - magnitude, 0.5f * getHeight() + magnitude);
        
        if (isBeat(x))
        {
            g.setColour(juce::Colours::white);
            g.drawVerticalLine(x, 0, getHeight());
        }
    }
}


void WaveformComponent::prepare(TrackData t)
{
    reset();
    track = t;
}


void WaveformComponent::pushBuffer(const float* audio, int numSamples)
{
    if (audio == nullptr) return;
    
    int numFrames = numSamples / WAVEFORM_FRAME_SIZE;
    
    for (int i = 0; i < numFrames; i++)
        pushFrame(&audio[i*WAVEFORM_FRAME_SIZE]);
    
    repaint();
}


void WaveformComponent::reset()
{
    filterLow.reset();
    filterMid.reset();
    filterHigh.reset();
    
    processBuffers.clear();
    
    levels.clear();
    colours.clear();
}


void WaveformComponent::pushFrame(const float* audio)
{
    if (levels.size() >= getWidth())
    {
        levels.remove(0);
        colours.remove(0);
    }
    
    // Copy frame data into filter buffers
    memcpy(processBuffers.getWritePointer(0), audio, WAVEFORM_FRAME_SIZE * sizeof(float));
    memcpy(processBuffers.getWritePointer(1), audio, WAVEFORM_FRAME_SIZE * sizeof(float));
    memcpy(processBuffers.getWritePointer(2), audio, WAVEFORM_FRAME_SIZE * sizeof(float));
    memcpy(processBuffers.getWritePointer(3), audio, WAVEFORM_FRAME_SIZE * sizeof(float));
    
    // Apply filters to buffers 1-3
    filterLow.processSamples(processBuffers.getWritePointer(1), WAVEFORM_FRAME_SIZE);
    filterMid.processSamples(processBuffers.getWritePointer(2), WAVEFORM_FRAME_SIZE);
    filterHigh.processSamples(processBuffers.getWritePointer(3), WAVEFORM_FRAME_SIZE);
    
    // Convert to absolute values
    juce::FloatVectorOperations::abs(processBuffers.getWritePointer(0), processBuffers.getReadPointer(0), WAVEFORM_FRAME_SIZE);
    juce::FloatVectorOperations::abs(processBuffers.getWritePointer(1), processBuffers.getReadPointer(1), WAVEFORM_FRAME_SIZE);
    juce::FloatVectorOperations::abs(processBuffers.getWritePointer(2), processBuffers.getReadPointer(2), WAVEFORM_FRAME_SIZE);
    juce::FloatVectorOperations::abs(processBuffers.getWritePointer(3), processBuffers.getReadPointer(3), WAVEFORM_FRAME_SIZE);
    
    levels.add(juce::FloatVectorOperations::findMaximum(processBuffers.getReadPointer(0), WAVEFORM_FRAME_SIZE));
    
    float low = juce::FloatVectorOperations::findMaximum(processBuffers.getReadPointer(1), WAVEFORM_FRAME_SIZE);
    float mid = juce::FloatVectorOperations::findMaximum(processBuffers.getReadPointer(2), WAVEFORM_FRAME_SIZE);
    float high = juce::FloatVectorOperations::findMaximum(processBuffers.getReadPointer(3), WAVEFORM_FRAME_SIZE);
    
    float multiplier = 2.f;
    
    low *= multiplier;
    mid *= multiplier;
    high *= multiplier;
    
    low += 0.1f;
    mid += 0.1f;
    high += 0.1f;
    
    low = juce::jmin(low, 1.f);
    mid = juce::jmin(mid, 1.f);
    high = juce::jmin(high, 1.f);
    
    colours.add(juce::Colour(low*255, mid*255, high*255));
}


bool WaveformComponent::isBeat(int frameIndex)
{
    if (track.bpm == -1 || track.beatPhase == -1) return false;
    
    int frameStart = frameIndex * WAVEFORM_FRAME_SIZE;
    int frameEnd = frameStart + WAVEFORM_FRAME_SIZE - 1;
    
    double beatLength = 60 * SUPPORTED_SAMPLERATE / track.bpm;
    
    frameStart -= track.beatPhase;
    frameEnd -= track.beatPhase;
    
    if (floor(frameEnd/beatLength) - floor(frameStart/beatLength) > 0)
        return true;
    
    return false;
}
