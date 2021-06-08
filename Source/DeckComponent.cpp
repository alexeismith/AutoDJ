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
    
    colourBackground = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
}


void DeckComponent::resized()
{
    waveform->setSize(getWidth(), WAVEFORM_VIEW_HEIGHT);
    
    if (deckId == 0)
    {
        waveform->setTopLeftPosition(0, getHeight() - waveform->getHeight());
        
        titlePosY = waveform->getY() - 30;
        infoPosY = titlePosY - 30;
    }
    else
    {
        waveform->setTopLeftPosition(0, 0);
        
        titlePosY = waveform->getY() + waveform->getHeight() + 30;
        infoPosY = titlePosY + 30;
    }
}


void DeckComponent::paint(juce::Graphics& g)
{
    int startY, endY;
    
    if (deckId == 0)
    {
        startY = waveform->getY();
        endY = startY - (getHeight() - waveform->getHeight() - 20);
    }
    else
    {
        startY = waveform->getY() + waveform->getHeight();
        endY = startY + (getHeight() - waveform->getHeight() - 10);
    }
    
    g.setGradientFill(juce::ColourGradient(colourBackground.withBrightness(0.17f), 0, startY, colourBackground, 0, endY, false));
    g.fillAll();
    
    if (!ready.load()) return;
    
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(24));
    g.drawText(title, 20, titlePosY, getWidth(), 1, juce::Justification::left);
    g.setFont(juce::Font(18));
    g.drawText(info, 20, infoPosY, getWidth(), 1, juce::Justification::left);
}


void DeckComponent::update()
{
    bool mixEnd;
    
    if (!trackProcessor->isReady(mixEnd))
    {
        if (mixEnd)
            reset();
        else
            waveform->reset();
        
        return;
    }
    
    Track* newTrack = trackProcessor->getNewTrack();
    if (newTrack)
        load(newTrack);
    
    if (trackProcessor->isLeader() && !track.leader)
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


void DeckComponent::reset()
{
    waveform->clearMarkers();
    waveform->reset();
    
    title.clear();
    info.clear();
    
    ready.store(false);
}


void DeckComponent::setMixMarkers()
{
    ready.store(false);
    
    int start, end;
    
    MixInfo mix = trackProcessor->getCurrentMix();
    
    waveform->clearMarkers();
    
    if (track.leader)
    {
        start = mix.leaderStart;
        end = mix.leaderEnd;
    }
    else
    {
        start = mix.followerStart;
        end = mix.followerEnd;
    }
    
    waveform->insertMarker(start);
    waveform->insertMarker(end);
    
    waveform->load(&track, true);
    
    ready.store(true);
}
