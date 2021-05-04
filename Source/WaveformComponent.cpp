//
//  WaveformComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 08/04/2021.
//

#include "WaveformComponent.hpp"

#include "CommonDefs.hpp"

#define WAVEFORM_BEAT_MARKER_SIZE (0.1f)


WaveformComponent::WaveformComponent()
{
    ready.store(false);
    
    filterLow.setCoefficients(juce::IIRCoefficients::makeLowPass(SUPPORTED_SAMPLERATE, 200, 1.0));
    filterMid.setCoefficients(juce::IIRCoefficients::makeBandPass(SUPPORTED_SAMPLERATE, 500, 1.0));
    filterHigh.setCoefficients(juce::IIRCoefficients::makeHighPass(SUPPORTED_SAMPLERATE, 10000, 1.0));
    
    setBufferedToImage(true);
    setOpaque(true);
    
    setSize(0, WAVEFORM_HEIGHT);
}


void WaveformComponent::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::black);
    g.fillAll();
    
    if (!ready.load()) return;

    g.drawImageAt(imageScaled, imageOffset, 0);
    
    g.setColour(juce::Colours::black.withAlpha(1.f - brightness));
    g.fillAll();
}


void WaveformComponent::resized()
{
    beatMarkerHeight = WAVEFORM_BEAT_MARKER_SIZE * getHeight();
}


void WaveformComponent::update(int playhead, double timeStretch, double gain)
{
    if (!ready.load()) return;
    
    brightness = juce::jmax(float(std::sqrt(gain)), 0.3f);
    
    int visibleWidth = getWidth() * timeStretch;
    
    double playheadAdjust = playhead - double(WAVEFORM_FRAME_SIZE * visibleWidth)/2;
    
    startFrame = round(playheadAdjust / WAVEFORM_FRAME_SIZE);
    
    imageScaled = image.getClippedImage(juce::Rectangle<int>(startFrame, 0, visibleWidth, getHeight()));
    
    imageOffset = round(double(visibleWidth - imageScaled.getWidth()) / timeStretch);
    
    imageScaled = imageScaled.rescaled(getWidth() - imageOffset, getHeight(), juce::Graphics::ResamplingQuality::mediumResamplingQuality);
    
    getCachedComponentImage()->invalidateAll();
}


void WaveformComponent::loadTrack(Track* t)
{
    int numSamples;
    
    track = t;
    
    numSamples = track->audio->getNumSamples();
    numFrames = numSamples / WAVEFORM_FRAME_SIZE;
    
    processBuffers.setSize(4, numSamples);
    
    // Copy frame data into filter buffers
    memcpy(processBuffers.getWritePointer(0), track->audio->getReadPointer(0), numSamples * sizeof(float));
    memcpy(processBuffers.getWritePointer(1), track->audio->getReadPointer(0), numSamples * sizeof(float));
    memcpy(processBuffers.getWritePointer(2), track->audio->getReadPointer(0), numSamples * sizeof(float));
    memcpy(processBuffers.getWritePointer(3), track->audio->getReadPointer(0), numSamples * sizeof(float));
    
    // Apply filters to buffers 1-3
    filterLow.processSamples(processBuffers.getWritePointer(1), numSamples);
    filterMid.processSamples(processBuffers.getWritePointer(2), numSamples);
    filterHigh.processSamples(processBuffers.getWritePointer(3), numSamples);
    
    // Convert to absolute values
    juce::FloatVectorOperations::abs(processBuffers.getWritePointer(0), processBuffers.getReadPointer(0), numSamples);
    juce::FloatVectorOperations::abs(processBuffers.getWritePointer(1), processBuffers.getReadPointer(1), numSamples);
    juce::FloatVectorOperations::abs(processBuffers.getWritePointer(2), processBuffers.getReadPointer(2), numSamples);
    juce::FloatVectorOperations::abs(processBuffers.getWritePointer(3), processBuffers.getReadPointer(3), numSamples);
    
    for (int i = 0; i < numFrames; i++)
        pushFrame(i);
    
    draw();
    
    ready.store(true);
}


void WaveformComponent::draw()
{
    image = juce::Image(juce::Image::RGB, numFrames, getHeight(), true);
    juce::Graphics g(image);
    
    float magnitude;
    bool downbeat;
    
    g.setColour(juce::Colours::black);
    g.fillAll();
    
    for (int frame = 0; frame < numFrames; frame++)
    {
        if (frame > 0 && frame < numFrames)
        {
            magnitude = levels[frame] * getHeight() * 0.4f;
            
            g.setColour(colours[frame]);
            g.drawVerticalLine(frame, 0.5f * getHeight() - magnitude, 0.5f * getHeight() + magnitude);
        }
        else
        {
            g.setColour(juce::Colours::darkgrey);
            g.drawVerticalLine(frame, 0, getHeight());
        }
        
        if (isBeat(frame, downbeat))
        {
            g.setColour(juce::Colours::white);
            
            if (downbeat)
            {
                g.drawVerticalLine(frame, 0, getHeight());
                g.drawVerticalLine(frame+1, 0, getHeight());
            }
            else
            {
                g.drawVerticalLine(frame, 0, beatMarkerHeight);
                g.drawVerticalLine(frame, getHeight() - beatMarkerHeight, getHeight());
                g.drawVerticalLine(frame+1, 0, beatMarkerHeight);
                g.drawVerticalLine(frame+1, getHeight() - beatMarkerHeight, getHeight());
            }
        }
    }
}


void WaveformComponent::reset()
{
    ready.store(false);
    
    getCachedComponentImage()->invalidateAll();
    
    filterLow.reset();
    filterMid.reset();
    filterHigh.reset();
    
    levels.clear();
    colours.clear();
    
    numFrames = startFrame = 0;
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


bool WaveformComponent::isBeat(int frameIndex, bool& downbeat)
{
    int remainder, frameStart, frameEnd;
    
    downbeat = false;
    
    if (track->info->bpm == -1) return false;
    
    frameStart = frameIndex * WAVEFORM_FRAME_SIZE;
    frameEnd = frameStart + WAVEFORM_FRAME_SIZE - 1;
    
    double beatLength = 60 * SUPPORTED_SAMPLERATE / track->info->bpm;
    
    frameStart -= track->info->beatPhase;
    frameEnd -= track->info->beatPhase;
    
    if (abs(floor(frameEnd/beatLength) - floor(frameStart/beatLength)) > 0)
    {
        remainder = (int(floor(frameEnd/beatLength)) - track->info->downbeat) % BEATS_PER_BAR;
        
        if (remainder == 0 || remainder == BEATS_PER_BAR)
            downbeat = true;
        
        return true;
    }
    
    return false;
}
