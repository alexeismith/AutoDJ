//
//  WaveformBarComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 04/05/2021.
//

#include "WaveformBarComponent.hpp"

WaveformBarComponent::WaveformBarComponent()
{
    setSize(0, WAVEFORM_BAR_HEIGHT);
}


void WaveformBarComponent::paint(juce::Graphics& g)
{
    if (!ready.load())
    {
        g.setColour(juce::Colours::black);
        g.fillAll();
        return;
    }
    
    g.drawImageAt(imageScaled, 0, 0);
    
    g.setColour(juce::Colours::white);
    g.drawRect(windowStartX, 0, windowWidth, getHeight());
}


void WaveformBarComponent::resized()
{
    imageScaled = image.rescaled(getWidth(), getHeight(), juce::Graphics::ResamplingQuality::mediumResamplingQuality);
}


void WaveformBarComponent::update(int playhead, double timeStretch, double gain)
{
    if (!ready.load()) return;
    
    int visibleWidth = getWidth() * timeStretch;
    double playheadAdjust = playhead - double(WAVEFORM_FRAME_SIZE * visibleWidth)/2;
    double multiplier = double(getWidth()) / numFrames;
    
    int windowStartFrame = playheadAdjust / WAVEFORM_FRAME_SIZE;
    windowStartX = round(windowStartFrame * multiplier);
    windowWidth = visibleWidth * multiplier;
    
    getCachedComponentImage()->invalidateAll();
}


void WaveformBarComponent::draw()
{
    WaveformComponent::draw();
    
    imageScaled = image.rescaled(getWidth(), getHeight(), juce::Graphics::ResamplingQuality::mediumResamplingQuality);
}


bool WaveformBarComponent::isBeat(int frameIndex, bool& downbeat)
{
    downbeat = false;
    return false;
}


void WaveformLoadThread::load(WaveformComponent* wave, WaveformBarComponent* waveBar, Track* t)
{
    waveform = wave;
    bar = waveBar;
    track = t;
    
    waveform->reset();
    bar->reset();
    
    startThread();
}
