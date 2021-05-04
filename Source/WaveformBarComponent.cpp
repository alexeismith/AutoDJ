//
//  WaveformBarComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 04/05/2021.
//

#include "WaveformBarComponent.hpp"

#define WAVEFORM_BAR_FRAME_SIZE (100000)

WaveformBarComponent::WaveformBarComponent()
{
    setSize(0, WAVEFORM_BAR_HEIGHT);
    
    frameSize = WAVEFORM_BAR_FRAME_SIZE;
}


void WaveformBarComponent::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::darkgrey);
    g.fillAll();
    
    if (!ready.load()) return;

    g.drawImageAt(imageScaled, 0, 0);
}


void WaveformBarComponent::update(int playhead, double timeStretch, double gain)
{
    if (!ready.load()) return;
    
    
    
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
    startThread();
}
