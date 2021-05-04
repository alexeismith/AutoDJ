//
//  LibraryComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/02/2021.
//

#include "LibraryView.hpp"


LibraryView::LibraryView(TrackDataManager* dm, juce::Button* play)
{
    dataManager = dm;
    playBtn = play;
    
    startTimerHz(30);
    
    trackTable.reset(new TrackTableComponent());
    addChildComponent(trackTable.get());
    trackTable->addColumns();
    
    chooseFolderBtn.reset(new juce::TextButton("Choose Folder"));
    chooseFolderBtn->setComponentID(juce::String(ComponentIDs::chooseFolderBtn));
    addAndMakeVisible(chooseFolderBtn.get());
    chooseFolderBtn->addListener(this);
    
    loadingFilesProgress.reset(new juce::ProgressBar(loadingProgress));
    addChildComponent(loadingFilesProgress.get());
    
    analysisProgress.reset(new juce::ProgressBar(loadingProgress));
    addChildComponent(analysisProgress.get());
    analysisProgress->setColour(analysisProgress->backgroundColourId, juce::Colours::darkgrey);
    
    waveform.reset(new WaveformComponent());
    waveformBar.reset(new WaveformBarComponent());
//    addAndMakeVisible(waveformBar.get());
    waveformLoader.reset(new WaveformLoadThread());
}

void LibraryView::resized()
{
    trackTable->setSize(getWidth(), getHeight());
    trackTable->setTopLeftPosition(0, 0);
    
    chooseFolderBtn->setSize(120, 40);
    chooseFolderBtn->setCentrePosition(getWidth()/2, (getHeight() + TOOLBAR_HEIGHT)/2);
    
    loadingFilesProgress->setSize(200, 20);
    loadingFilesProgress->setCentrePosition(getWidth()/2, getHeight()/2);
    
    analysisProgress->setSize(200, 20);
    analysisProgress->setTopLeftPosition(10, getHeight() - 30);
    
    waveformBar->setSize(getWidth(), 40);
}


void LibraryView::buttonClicked(juce::Button* button)
{
    int id = button->getComponentID().getIntValue();
    
    switch (id)
    {
        case ComponentIDs::chooseFolderBtn:
            chooseFolder();
            break;
        default:
            jassert(false); // Unrecognised button ID
    }
}


void LibraryView::timerCallback()
{
    if (waitingForFiles)
    {
        if (dataManager->isLoaded(loadingProgress))
        {
            waitingForFiles = false;
            loadFiles();
            info = dataManager->getTracks()[0];
            track.info = &info;
            track.audio = dataManager->loadAudio(info.getFilename());
            waveformLoader->load(waveform.get(), waveformBar.get(), &track);
        }
    }
    else if (waitingForAnalysis)
    {
        bool canStartPlaying;
        
        if (dataManager->analysisProgress(loadingProgress, canStartPlaying))
        {
            trackTable->sort();
            waitingForAnalysis = false;
            analysisProgress->setVisible(false);
        }
        
        if (canStartPlaying)
            playBtn->setEnabled(true);
    }
    
    if (dataManager->trackDataUpdate.load())
    {
        dataManager->trackDataUpdate.store(false);
        trackTable->sort();
    }
    
//    waveformBar->update(0);
//    repaint();
}


void LibraryView::chooseFolder()
{
    juce::FileChooser chooser ("Choose Music Folder");
    if (chooser.browseForDirectory())
    {
        dataManager->initialise(chooser.getResult());
        
        waitingForFiles = true;
        
        chooseFolderBtn->setVisible(false);
        loadingFilesProgress->setVisible(true);
    }
}


void LibraryView::loadFiles()
{
    trackTable->populate(dataManager->getTracks(), dataManager->getNumTracks());

    loadingFilesProgress->setVisible(false);
    analysisProgress->setVisible(true);
    trackTable->setVisible(true);
    playBtn->setVisible(true);
    
    loadingProgress = 0.0;
    waitingForAnalysis = true;
}
