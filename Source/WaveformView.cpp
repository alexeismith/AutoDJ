//
//  WaveformView.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 06/05/2021.
//

#include "WaveformView.hpp"



WaveformView::WaveformView(DataManager* dm, bool clickToScroll, bool scrollBarAtBottom, bool hideWhenEmpty)
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
    scrollBar->setSize(getWidth(), WAVEFORM_SCROLL_BAR_HEIGHT);
    
    if (scrollBarBottom)
    {
        waveform->setTopLeftPosition(0, 0);
        scrollBar->setTopLeftPosition(0, WAVEFORM_HEIGHT);
    }
    else
    {
        scrollBar->setTopLeftPosition(0, 0);
        waveform->setTopLeftPosition(0, WAVEFORM_SCROLL_BAR_HEIGHT);
    }
}


void WaveformView::load(Track* track, bool force)
{
    trackLength = track->info->getLengthSamples();
    loader->load(track, force);
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


void WaveformView::insertMarker(int sample)
{
    waveform->insertMarker(sample);
    scrollBar->insertMarker(sample, 40);
}


void WaveformView::clearMarkers()
{
    waveform->clearMarkers();
    scrollBar->clearMarkers();
}


void WaveformView::scroll(juce::Point<int> pos)
{
    if (!scrollable) return;
    
    if (!scrolling)
        if (!scrollBar->getBounds().contains(pos)) return;
    
    scrolling = true;
    
    double xPos = juce::jlimit(0, getWidth() - scrollBar->getWindowWidth()/2 - 3, pos.getX());
    
    double proportionX = xPos / getWidth();
    update(trackLength * proportionX);
    repaint();
}
