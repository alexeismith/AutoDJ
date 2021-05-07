//
//  WaveformScrollBar.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 04/05/2021.
//

#include "WaveformScrollBar.hpp"

WaveformScrollBar::WaveformScrollBar()
{
    setSize(0, WAVEFORM_BAR_HEIGHT);
}


void WaveformScrollBar::paint(juce::Graphics& g)
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


void WaveformScrollBar::resized()
{
    imageScaled = image.rescaled(getWidth(), getHeight(), juce::Graphics::ResamplingQuality::mediumResamplingQuality);
}


void WaveformScrollBar::update(int playhead, double timeStretch, double gain)
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


void WaveformScrollBar::draw(juce::Array<juce::Colour>* colours, juce::Array<float>* levels)
{
    WaveformComponent::draw(colours, levels);
    
    imageScaled = image.rescaled(getWidth(), getHeight(), juce::Graphics::ResamplingQuality::mediumResamplingQuality);
}


bool WaveformScrollBar::isBeat(int frameIndex, bool& downbeat)
{
    downbeat = false;
    
    // TODO: temp
    int frameStart = frameIndex * WAVEFORM_FRAME_SIZE;
    int frameEnd = frameStart + WAVEFORM_FRAME_SIZE - 1;
    for (int marker : markers)
    {
        if (marker >= frameStart && marker <= frameEnd)
        {
            downbeat = true;
            break;
        }
    }
    
    return false;
}
