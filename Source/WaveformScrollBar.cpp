//
//  WaveformScrollBar.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 04/05/2021.
//

#include "WaveformScrollBar.hpp"

WaveformScrollBar::WaveformScrollBar()
{
    setSize(0, WAVEFORM_SCROLL_BAR_HEIGHT);
}


void WaveformScrollBar::resized()
{
    imageScaled = image.rescaled(getWidth(), getHeight(), juce::Graphics::ResamplingQuality::mediumResamplingQuality);
}


void WaveformScrollBar::paint(juce::Graphics& g)
{
    if (!ready.load())
    {
        g.setColour(colourBackground);
        g.fillAll();
        return;
    }
    
    g.drawImageAt(imageScaled, 0, 0);
    
    g.setColour(juce::Colours::white);
    g.drawRect(windowStartX, 0, windowWidth, getHeight());
}


void WaveformScrollBar::update(int playhead, double timeStretch, double gain)
{
    if (!ready.load()) return;
    
    // No stretching needed, so update is much simpler than WaveformComponent::update()
    // Although the whole waveform is shown, we still need to calculate the visible width
    // of the zoomed waveform, so we can draw a white window around the corresponding area in the track
    
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
    
    // For scrollbar, waveform is simply scaled to the width of the component, so we can see it all
    imageScaled = image.rescaled(getWidth(), getHeight(), juce::Graphics::ResamplingQuality::mediumResamplingQuality);
}


bool WaveformScrollBar::isBeat(int frameIndex, bool& downbeat)
{
    // Don't want to draw any beat markers in scroll bar waveform, so exit
    downbeat = false;
    return false;
}
