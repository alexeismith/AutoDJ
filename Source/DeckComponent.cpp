//
//  DeckComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/04/2021.
//

#include "DeckComponent.hpp"

#include "CamelotKey.hpp"


DeckComponent::DeckComponent(int id, TrackProcessor* processor) :
    deckId(id), trackProcessor(processor)
{
    waveform.reset(new WaveformView(nullptr, false, id, false));
    addAndMakeVisible(waveform.get());
}


void DeckComponent::paint(juce::Graphics& g)
{
    if (!ready.load()) return;
    
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(24));
    g.drawText(title, 20, titlePosY, getWidth(), 1, juce::Justification::left);
    g.setFont(juce::Font(18));
    g.drawText(info, 20, infoPosY, getWidth(), 1, juce::Justification::left);
}


void DeckComponent::resized()
{
    waveform->setSize(getWidth(), WAVEFORM_VIEW_HEIGHT);
    
    if (deckId == 0)
    {
        waveform->setTopLeftPosition(0, getHeight() - WAVEFORM_VIEW_HEIGHT);
        
        titlePosY = waveform->getY() - 30;
        infoPosY = titlePosY - 30;
    }
    else
    {
        waveform->setTopLeftPosition(0, 0);
        
        titlePosY = waveform->getY() + WAVEFORM_VIEW_HEIGHT + 30;
        infoPosY = titlePosY + 30;
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
    
    if (!track.leader && playhead > track.getCurrentMix()->endNext)
    {
        track.leader = true;
        setMixMarkers();
    }
    
    if (ready.load())
    {
        waveform->update(playhead, trackProcessor->getTimeStretch(), trackProcessor->getTrack()->gain.currentValue);
    }
}


void DeckComponent::logPlayheadPosition()
{
    if (ready.load())
        playhead = trackProcessor->getTrack()->getPlayhead();
}


void DeckComponent::load(Track* trackPtr)
{
    ready.store(false);
    
    track = *trackPtr;
    
    title = track.info->getArtistTitle();
    info = "Length: " + AutoDJ::getLengthString(track.info->length) +  "    BPM: " + juce::String(track.info->bpm) +  "    Key: " + CamelotKey(track.info->key).getName() +  "    Groove: " + AutoDJ::getGrooveString(track.info->groove);
    
    waveform->load(&track);
    
    setMixMarkers();
    
    ready.store(true);
}


void DeckComponent::setMixMarkers()
{
    ready.store(false);
    
    waveform->clearMarkers();
    
    int start, end;
    
    if (track.leader)
    {
        start = track.getCurrentMix()->start;
        end = track.getCurrentMix()->end;
    }
    else
    {
        start = track.getCurrentMix()->startNext;
        end = track.getCurrentMix()->endNext;
    }
    
    waveform->insertMarker(start);
    waveform->insertMarker(end);
    
    waveform->load(&track);
    
    ready.store(true);
}
