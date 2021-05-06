//
//  WaveformView.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 06/05/2021.
//

#include "WaveformView.hpp"



WaveformView::WaveformView(bool scrollBarAtBottom)
{
    scrollBarBottom = scrollBarAtBottom;
    
    waveform.reset(new WaveformComponent());
    addAndMakeVisible(waveform.get());
    
    scrollBar.reset(new WaveformScrollBar());
    addAndMakeVisible(scrollBar.get());
    
    loader.reset(new WaveformLoader(waveform.get(), scrollBar.get()));
}


void WaveformView::resized()
{
    waveform->setSize(getWidth(), WAVEFORM_HEIGHT);
    scrollBar->setSize(getWidth(), WAVEFORM_BAR_HEIGHT);
    
    if (scrollBarBottom)
    {
        waveform->setTopLeftPosition(0, 0);
        scrollBar->setTopLeftPosition(0, WAVEFORM_HEIGHT);
    }
    else
    {
        scrollBar->setTopLeftPosition(0, 0);
        waveform->setTopLeftPosition(0, WAVEFORM_BAR_HEIGHT);
    }
}


void WaveformView::load(Track* track)
{
    loader->load(track);
}


void WaveformView::update(int playhead, double timeStretch, double gain)
{
    waveform->update(playhead, timeStretch, gain);
    scrollBar->update(playhead, timeStretch);
}


void WaveformView::reset()
{
    waveform->reset();
    scrollBar->reset();
}
