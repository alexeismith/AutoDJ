//
//  WaveformComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 08/04/2021.
//

#include "WaveformComponent.hpp"

#include "CommonDefs.hpp"


#define WAVEFORM_BAR_HEIGHT (0.1f)


WaveformComponent::WaveformComponent(int width, TrackDataManager* dm) :
    dataManager(dm)
{
    setSize(width, 300);
    
    filterLow.setCoefficients(juce::IIRCoefficients::makeLowPass(SUPPORTED_SAMPLERATE, 200, 1.0));
    filterMid.setCoefficients(juce::IIRCoefficients::makeBandPass(SUPPORTED_SAMPLERATE, 500, 1.0));
    filterHigh.setCoefficients(juce::IIRCoefficients::makeHighPass(SUPPORTED_SAMPLERATE, 10000, 1.0));
    
    startTimerHz(60);
}


void WaveformComponent::paint(juce::Graphics& g)
{
    int frame, barHeight = WAVEFORM_BAR_HEIGHT * getHeight();
    float magnitude;
    
    g.setColour(juce::Colours::black);
    g.fillAll();
    
    for (int x = 0; x < getWidth(); x++)
    {
        frame = x + startFrame;
        
        if (frame > 0 && frame < numFrames)
        {
            magnitude = levels[frame] * getHeight() * 0.4f;
            
            g.setColour(colours[frame]);
            g.drawVerticalLine(x, 0.5f * getHeight() - magnitude, 0.5f * getHeight() + magnitude);
        }
        
        if (isBeat(frame))
        {
            g.setColour(juce::Colours::white);
            g.drawVerticalLine(x, 0, barHeight);
            g.drawVerticalLine(x, getHeight() - barHeight, getHeight());
        }
    }
}


void WaveformComponent::loadTrack(TrackData t, int startSample)
{
    juce::AudioBuffer<float> audio;
    int numSamples;
    
    reset();
    
    track = t;
    startFrame = round(double(startSample) / WAVEFORM_FRAME_SIZE);
    
    dataManager->fetchAudio(track.filename, audio);
    
    numSamples = audio.getNumSamples();
    
    processBuffers.setSize(4, numSamples);
    
    // Copy frame data into filter buffers
    memcpy(processBuffers.getWritePointer(0), audio.getReadPointer(0), numSamples * sizeof(float));
    memcpy(processBuffers.getWritePointer(1), audio.getReadPointer(0), numSamples * sizeof(float));
    memcpy(processBuffers.getWritePointer(2), audio.getReadPointer(0), numSamples * sizeof(float));
    memcpy(processBuffers.getWritePointer(3), audio.getReadPointer(0), numSamples * sizeof(float));
    
    // Apply filters to buffers 1-3
    filterLow.processSamples(processBuffers.getWritePointer(1), numSamples);
    filterMid.processSamples(processBuffers.getWritePointer(2), numSamples);
    filterHigh.processSamples(processBuffers.getWritePointer(3), numSamples);
    
    // Convert to absolute values
    juce::FloatVectorOperations::abs(processBuffers.getWritePointer(0), processBuffers.getReadPointer(0), numSamples);
    juce::FloatVectorOperations::abs(processBuffers.getWritePointer(1), processBuffers.getReadPointer(1), numSamples);
    juce::FloatVectorOperations::abs(processBuffers.getWritePointer(2), processBuffers.getReadPointer(2), numSamples);
    juce::FloatVectorOperations::abs(processBuffers.getWritePointer(3), processBuffers.getReadPointer(3), numSamples);
    
    numFrames = numSamples / WAVEFORM_FRAME_SIZE;
    
    for (int i = 0; i < numFrames; i++)
        pushFrame(i);
    
    processBuffers.clear();
    
    repaint();
}


void WaveformComponent::scroll(int samples)
{
    int frames = round(double(samples + scrollRemainder) / WAVEFORM_FRAME_SIZE);
    
    startFrame += frames;
    
    scrollRemainder = (samples + scrollRemainder) - (frames * WAVEFORM_FRAME_SIZE);
    
    repaint();
}


//void WaveformComponent::analyse(juce::AudioBuffer<float> audio)
//{
//    int numFrames = audio.getNumSamples() / WAVEFORM_FRAME_SIZE;
//    
//    for (int i = 0; i < numFrames; i++)
//        pushFrame(&audio.getReadPointer(0)[i*WAVEFORM_FRAME_SIZE]);
//    
//    repaint();
//}


void WaveformComponent::reset()
{
    filterLow.reset();
    filterMid.reset();
    filterHigh.reset();
    
    levels.clear();
    colours.clear();
    
    numFrames = startFrame = scrollRemainder = 0;
}


void WaveformComponent::pushFrame(int index)
{
    int startSample = index * WAVEFORM_FRAME_SIZE;
    
    levels.add(juce::FloatVectorOperations::findMaximum(processBuffers.getReadPointer(0, startSample), WAVEFORM_FRAME_SIZE));
    
    float low = juce::FloatVectorOperations::findMaximum(processBuffers.getReadPointer(1, startSample), WAVEFORM_FRAME_SIZE);
    float mid = juce::FloatVectorOperations::findMaximum(processBuffers.getReadPointer(2, startSample), WAVEFORM_FRAME_SIZE);
    float high = juce::FloatVectorOperations::findMaximum(processBuffers.getReadPointer(3, startSample), WAVEFORM_FRAME_SIZE);
    
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
    
    if (abs(floor(frameEnd/beatLength) - floor(frameStart/beatLength)) > 0)
        return true;
    
    return false;
}
