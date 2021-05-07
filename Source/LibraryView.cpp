//
//  LibraryComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/02/2021.
//

#include "LibraryView.hpp"


LibraryView::LibraryView(TrackDataManager* dm, double* loadingProgress)
{
    dataManager = dm;
    
    startTimerHz(30);
    
    trackEditor.reset(new TrackEditor(dataManager));
    
    trackTable.reset(new TrackTableComponent(trackEditor.get()));
    addAndMakeVisible(trackTable.get());
    trackTable->addColumns();
    
    addAndMakeVisible(trackEditor.get());
    
    analysisProgress.reset(new AnalysisProgressBar(dataManager->getAnalysisManager(), *loadingProgress));
    addAndMakeVisible(analysisProgress.get());
    analysisProgress->setColour(analysisProgress->backgroundColourId, juce::Colours::darkgrey);
}

void LibraryView::resized()
{
    trackTable->setSize(getWidth(), getHeight() - WAVEFORM_VIEW_HEIGHT + 8);
    trackTable->setTopLeftPosition(0, 0);
    
    trackEditor->setSize(getWidth(), WAVEFORM_VIEW_HEIGHT);
    trackEditor->setTopLeftPosition(0, getHeight() - WAVEFORM_VIEW_HEIGHT);
    
    analysisProgress->setSize(300, 30);
    analysisProgress->setCentrePosition(getWidth()/2, getHeight() - 30);
}


void LibraryView::timerCallback()
{    
    if (dataManager->trackDataUpdate.load())
    {
        dataManager->trackDataUpdate.store(false);
        trackTable->sort();
    }
}


void LibraryView::loadFiles()
{
    trackTable->populate(dataManager->getTracks(), dataManager->getNumTracks());
}
