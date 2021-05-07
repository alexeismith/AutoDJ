//
//  TrackEditor.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 06/05/2021.
//

#include "TrackEditor.hpp"


TrackEditor::TrackEditor(TrackDataManager* dm) :
    dataManager(dm)
{
    waveform.reset(new WaveformView(dm, true, false, true));
    addAndMakeVisible(waveform.get());
    
    message = "Select a track to view or edit it here.";
    
    analyserSegments.reset(new AnalyserSegments());
}


void TrackEditor::resized()
{
    waveform->setTopLeftPosition(0, 0);
    waveform->setSize(getWidth(), getHeight());
    waveform->refresh();
}


void TrackEditor::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::black);
    g.fillAll();
    
    g.setColour(juce::Colours::grey);
    g.setFont(juce::Font(18));
    g.drawText(message, 0, 0, getWidth(), getHeight(), juce::Justification::centred);
}


void TrackEditor::load(Track t)
{
    track = t;
//    track.audio = nullptr;
    
    track.audio = dataManager->loadAudio(track.info->getFilename());
    
    juce::Array<int> segments = analyserSegments->analyse(track.audio);
    
    waveform->load(&track);
    waveform->update(0);
    waveform->clearMarkers();

    for (auto segment : segments)
        waveform->insertMarker(track.info->getNearestDownbeat(segment));
    
    message = "Loading...";
    
    repaint();
}
