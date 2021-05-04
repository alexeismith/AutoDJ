//
//  WaveformBarComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 04/05/2021.
//

#include "WaveformBarComponent.hpp"

#define WAVEFORM_BAR_FRAME_SIZE (50000)

WaveformBarComponent::WaveformBarComponent()
{
    frameSize = WAVEFORM_BAR_FRAME_SIZE;
}


void WaveformBarComponent::draw(int playhead, double timeStretch, double gain)
{
    if (!ready.load()) return;
    
    brightness = 1.0;
    
    drawWidth = numFrames;
    
    updateImage();
}

bool WaveformBarComponent::isBeat(int frameIndex, bool& downbeat)
{
    downbeat = false;
    return false;
}
