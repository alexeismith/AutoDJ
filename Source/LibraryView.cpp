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
    
    trackTable.reset(new TrackTableComponent());
    addAndMakeVisible(trackTable.get());
    trackTable->addColumns();
    
    analysisProgress.reset(new AnalysisProgressBar(dataManager->getAnalysisManager(), *loadingProgress));
    addAndMakeVisible(analysisProgress.get());
    analysisProgress->setColour(analysisProgress->backgroundColourId, juce::Colours::darkgrey);
}

void LibraryView::resized()
{
    trackTable->setSize(getWidth(), getHeight());
    trackTable->setTopLeftPosition(0, 0);
    
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
