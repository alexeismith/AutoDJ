//
//  WaveformComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 08/04/2021.
//

#include "WaveformComponent.hpp"

#include "CommonDefs.hpp"


#define WAVEFORM_BAR_HEIGHT (0.1f)


WaveformComponent::WaveformComponent()
{
    ready.store(false);
    
    filterLow.setCoefficients(juce::IIRCoefficients::makeLowPass(SUPPORTED_SAMPLERATE, 200, 1.0));
    filterMid.setCoefficients(juce::IIRCoefficients::makeBandPass(SUPPORTED_SAMPLERATE, 500, 1.0));
    filterHigh.setCoefficients(juce::IIRCoefficients::makeHighPass(SUPPORTED_SAMPLERATE, 10000, 1.0));
    
    images.add(new juce::Image());
    images.add(new juce::Image());
    
    setBufferedToImage(true);
    setOpaque(true);
}


void WaveformComponent::paint(juce::Graphics& g)
{
    if (!ready.load())
    {
        g.setColour(juce::Colours::black);
        g.fillAll();
        return;
    }

    g.drawImageAt(*images.getUnchecked(imageToPaint.load()), 0, 0);
}


void WaveformComponent::resized()
{
    barHeight = WAVEFORM_BAR_HEIGHT * getHeight();
}


void WaveformComponent::draw(int playhead, double timeStretch, double gain)
{
    if (!ready.load()) return;
    
    brightness = juce::jmax(float(std::sqrt(gain)), 0.3f);
    
    #ifdef WAVEFORM_HALF_RESOLUTION
        drawWidth = getWidth()/2 * (timeStretch);
    #else
        drawWidth = getWidth() * (timeStretch);
    #endif
    
    double playheadAdjust = playhead - double(WAVEFORM_FRAME_SIZE * drawWidth)/2;
    
    startFrame = round(playheadAdjust / WAVEFORM_FRAME_SIZE);
    
    updateImage();
}


void WaveformComponent::flipImage()
{
    imageToPaint.store(1 - imageToPaint.load());
    getCachedComponentImage()->invalidateAll();
}


void WaveformComponent::loadTrack(Track* t)
{
    int numSamples;
    
    reset();
    
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
    
    processBuffers.clear();
    
    ready.store(true);
}


void WaveformComponent::updateImage()
{
    int imageIndex = 1 - imageToPaint.load();
    juce::Image* image = images.getUnchecked(imageIndex);
    *image = juce::Image(juce::Image::RGB, drawWidth, getHeight(), true);
    juce::Graphics g(*image);
    
    int frame;
    float magnitude;
    bool downbeat;
    
    g.setColour(juce::Colours::black);
    g.fillAll();
    
    for (int x = 0; x < drawWidth; x++)
    {
        frame = x + startFrame;
        
        if (frame > 0 && frame < numFrames)
        {
            magnitude = levels[frame] * getHeight() * 0.4f;
            
            g.setColour(colours[frame].withAlpha(brightness));
            g.drawVerticalLine(x, 0.5f * getHeight() - magnitude, 0.5f * getHeight() + magnitude);
        }
        else
        {
            g.setColour(juce::Colours::darkgrey);
            g.drawVerticalLine(x, 0, getHeight());
        }
        
        if (isBeat(frame, downbeat))
        {
            g.setColour(juce::Colours::white);
            
            if (downbeat)
            {
                g.drawVerticalLine(x, 0, getHeight());
                g.drawVerticalLine(x+1, 0, getHeight());
            }
            else
            {
                g.drawVerticalLine(x, 0, barHeight);
                g.drawVerticalLine(x, getHeight() - barHeight, getHeight());
                g.drawVerticalLine(x+1, 0, barHeight);
                g.drawVerticalLine(x+1, getHeight() - barHeight, getHeight());
            }
        }
    }
    
    *image = image->rescaled(getWidth(), getHeight(), juce::Graphics::ResamplingQuality::mediumResamplingQuality);
}


void WaveformComponent::reset()
{
    ready.store(false);
    
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
    
    if (track->info.bpm == -1) return false;
    
    frameStart = frameIndex * WAVEFORM_FRAME_SIZE;
    frameEnd = frameStart + WAVEFORM_FRAME_SIZE - 1;
    
    double beatLength = 60 * SUPPORTED_SAMPLERATE / track->info.bpm;
    
    frameStart -= track->info.beatPhase;
    frameEnd -= track->info.beatPhase;
    
    if (abs(floor(frameEnd/beatLength) - floor(frameStart/beatLength)) > 0)
    {
        remainder = (int(floor(frameEnd/beatLength)) - track->info.downbeat) % BEATS_PER_BAR;
        
        if (remainder == 0 || remainder == BEATS_PER_BAR)
            downbeat = true;
        
        return true;
    }
    
    return false;
}
