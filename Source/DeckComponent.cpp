//
//  DeckComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/04/2021.
//

#include "DeckComponent.hpp"


DeckComponent::DeckComponent(int id, TrackProcessor* processor) :
    deckId(id), trackProcessor(processor)
{
    waveform.reset(new WaveformComponent());
    addAndMakeVisible(waveform.get());
    
    waveformBar.reset(new WaveformBarComponent());
    addAndMakeVisible(waveformBar.get());
    
    waveformLoader.reset(new WaveformLoadThread());
}


void DeckComponent::resized()
{
    waveform->setSize(getWidth(), WAVEFORM_HEIGHT);
    waveformBar->setSize(getWidth(), WAVEFORM_BAR_HEIGHT);
    
    if (deckId == 0)
    {
        waveform->setTopLeftPosition(0, getHeight()-WAVEFORM_HEIGHT);
        waveformBar->setTopLeftPosition(0, waveform->getY()-WAVEFORM_BAR_HEIGHT);
    }
    else
    {
        waveform->setTopLeftPosition(0, 0);
        waveformBar->setTopLeftPosition(0, waveform->getY()+WAVEFORM_HEIGHT);
    }
}


void DeckComponent::buttonClicked(juce::Button* button)
{
    int id = button->getComponentID().getIntValue();
    
    switch (id)
    {
        default:
            jassert(false); // Unrecognised button ID
    }
}


void DeckComponent::load(Track* trackPtr)
{
    track = *trackPtr;
    
//    DBG("Deck " << deckId+1 << " playing: " << track.info.getFilename());
    
    waveformLoader->load(waveform.get(), waveformBar.get(), &track);
    
    trackLoaded = true;
    
    // TODO: show title, artist, etc
}


void DeckComponent::update()
{
    if (!trackProcessor->isReady())
    {
        waveform->reset();
        return;
    }
    
    Track* t = trackProcessor->getNewTrack();
    if (t)
        load(t);
    
    if (trackLoaded)
    {
        waveform->update(playhead, trackProcessor->getTimeStretch(), trackProcessor->getTrack()->gain.currentValue);
        waveformBar->update(playhead, trackProcessor->getTimeStretch());
    }
       
}


void DeckComponent::logPlayheadPosition()
{
    if (trackLoaded)
        playhead = trackProcessor->getTrack()->getPlayhead();
}
