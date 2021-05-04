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
    frameSize = WAVEFORM_BAR_FRAME_SIZE;
}


//void WaveformBarComponent::paint(juce::Graphics& g)
//{
//    WaveformComponent::paint(g);
//    
//    for ()
//}


void WaveformBarComponent::draw(int playhead, double timeStretch, double gain)
{
    if (!ready.load()) return;
    
    brightness = 1.0;
    
    drawWidth = numFrames;
    
    int windowWidth = getWidth() * (timeStretch);
    
    double playheadAdjust = playhead - double(WAVEFORM_FRAME_SIZE * windowWidth)/2;
    
    windowStartFrame = round(playheadAdjust / WAVEFORM_FRAME_SIZE);
    windowEndFrame = windowStartFrame + windowWidth;
    
//    DBG("windowStartFrame: " << windowStartFrame << " windowEndFrame: " << windowEndFrame);
    
    double scale = double(WAVEFORM_FRAME_SIZE) / WAVEFORM_BAR_FRAME_SIZE;
    windowStartFrame = round(windowStartFrame * scale);
    windowEndFrame = round(windowEndFrame * scale);
    
//    DBG("ADJUST windowStartFrame: " << windowStartFrame << " windowEndFrame: " << windowEndFrame);
    
    updateImage();
}

bool WaveformBarComponent::isBeat(int frameIndex, bool& downbeat)
{
    if (frameIndex == windowStartFrame || frameIndex == windowEndFrame)
        downbeat = true;
    else
        downbeat = false;
    
    if (frameIndex > windowStartFrame && frameIndex < windowEndFrame)
        return true;
    
    return false;
}


void WaveformLoadThread::load(WaveformComponent* wave, WaveformBarComponent* waveBar, Track* t)
{
    waveform = wave;
    bar = waveBar;
    track = t;
    startThread();
}
