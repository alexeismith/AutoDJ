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
    
    analysisManager.reset(new AnalysisManager(dataManager));
    
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
    
    trackDataUpdate.store(false);
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
        }
    }
    else if (waitingForAnalysis)
    {
        if (analysisManager->isFinished(loadingProgress))
        {
            waitingForAnalysis = false;
            analysisProgress->setVisible(false);
            playBtn->setEnabled(true);
        }
        else if (analysisManager->minimumAnalysed())
        {
            playBtn->setEnabled(true);
        }
    }
    
    if (trackDataUpdate.load())
    {
        trackDataUpdate.store(false);
        trackTable->refresh();
    }
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
    trackTable->populate(dataManager->getTracks());

    loadingFilesProgress->setVisible(false);
    analysisProgress->setVisible(true);
    trackTable->setVisible(true);
    playBtn->setVisible(true);
    
    loadingProgress = 0.0;
    
    analysisManager->startAnalysis();
    waitingForAnalysis = true;
    
    
    
    
//    TrackInfo track = dataManager->getTracks()->getReference(0);
//
//    DBG("Waveform: " << track.filename);
//    waveform->loadTrack(track);
//    waveform->setVisible(true);
//    waveform->scroll(1800000);
//    DBG("Waveform Done");
    
//    audioProcessor->play(track);
    
//    audioProcessor->getTrackProcessor()->newTempoShift(10, 44100 * 6);
//    audioProcessor->getTrackProcessor()->newPitchShift(2, 44100 * 20);
    
//    juce::Thread::getCurrentThread()->sleep(3000);
//
//    audioProcessor->preview(dataManager->getTracks()->getReference(1), 0, 44100*2);
//
//    juce::Thread::getCurrentThread()->sleep(3000);
//
//    audioProcessor->play();
}
