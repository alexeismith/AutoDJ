//
//  DeckComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/04/2021.
//

#include "DeckComponent.hpp"


DeckComponent::DeckComponent(TrackProcessor* processor, bool top) :
    topDeck(top), trackProcessor(processor)
{
    waveform.reset(new WaveformComponent());
    addAndMakeVisible(waveform.get());
}


void DeckComponent::resized()
{
    waveform->setSize(getWidth(), WAVEFORM_HEIGHT);
    if (topDeck)
        waveform->setTopLeftPosition(0, getHeight()-WAVEFORM_HEIGHT);
    else
        waveform->setTopLeftPosition(0, 0);
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


void DeckComponent::update()
{
    Track* track = trackProcessor->getTrack();
    if (!track) return;
    
    if (currentTrackHash != track->info.hash)
    {
        waveform->loadTrack(track);
        currentTrackHash = track->info.hash;
    }
        
    waveform->update(track->playhead, trackProcessor->getTimeStretch(), track->gain.currentValue);
}
