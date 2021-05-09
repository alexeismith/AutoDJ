//
//  LibraryComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/02/2021.
//

#include "LibraryView.hpp"


LibraryView::LibraryView(TrackDataManager* dm)
{
    dataManager = dm;
    
    trackEditor.reset(new TrackEditor(dataManager));
    
    trackTable.reset(new TrackTableComponent(trackEditor.get()));
    addAndMakeVisible(trackTable.get());
    trackTable->addColumns();
    
    addAndMakeVisible(trackEditor.get());
}

void LibraryView::resized()
{
    trackTable->setSize(getWidth(), getHeight() - WAVEFORM_VIEW_HEIGHT + 8);
    trackTable->setTopLeftPosition(0, 0);
    
    trackEditor->setSize(getWidth(), WAVEFORM_VIEW_HEIGHT);
    trackEditor->setTopLeftPosition(0, getHeight() - WAVEFORM_VIEW_HEIGHT);
}


void LibraryView::loadFiles()
{
    trackTable->populate(dataManager->getTracks(), dataManager->getNumTracks());
}
