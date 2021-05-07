//
//  WaveformView.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 06/05/2021.
//

#include "WaveformView.hpp"



WaveformView::WaveformView(TrackDataManager* dm, bool clickToScroll, bool scrollBarAtBottom, bool hideWhenEmpty)
{
    scrollable = clickToScroll;
    scrollBarBottom = scrollBarAtBottom;
    
    waveform.reset(new WaveformComponent());
    addAndMakeVisible(waveform.get());
    
    scrollBar.reset(new WaveformScrollBar());
    addAndMakeVisible(scrollBar.get());
    
    loader.reset(new WaveformLoader(dm, waveform.get(), scrollBar.get(), hideWhenEmpty));
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
    trackLength = track->info->getLengthSamples();
    loader->load(track);
    update(0);
}


void WaveformView::update(int p, double t, double g)
{
    playhead = p;
    timeStretch = t;
    gain = g;
    
    waveform->update(playhead, timeStretch, gain);
    scrollBar->update(playhead, timeStretch);
}


void WaveformView::refresh()
{
    update(playhead, timeStretch, gain);
    repaint();
}


void WaveformView::reset()
{
    waveform->reset();
    scrollBar->reset();
}


void WaveformView::mouseDown(const juce::MouseEvent &event)
{
    if (!scrollable) return;
    
    double proportionX = double(event.getPosition().getX()) / getWidth();
    update(trackLength * proportionX);
}
