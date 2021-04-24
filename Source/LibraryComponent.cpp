//
//  LibraryComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/02/2021.
//

#include "LibraryComponent.hpp"


LibraryComponent::LibraryComponent(AudioProcessor* p, TrackDataManager* dm, juce::Button* play)
{
    audioProcessor = p;
    dataManager = dm;
    playBtn = play;
    
    startTimerHz(30);
    
    analysisManager.reset(new AnalysisManager(dataManager));
    
    trackTable.reset(new TrackTableComponent());
    addAndMakeVisible(trackTable.get());
    trackTable->setVisible(false);
    trackTable->addColumns();
    
    chooseFolderBtn.reset(new juce::TextButton("Choose Folder"));
    chooseFolderBtn->setComponentID(juce::String(ComponentIDs::chooseFolderBtn));
    addAndMakeVisible(chooseFolderBtn.get());
    chooseFolderBtn->addListener(this);
    
    waveform.reset(new WaveformComponent(800, dataManager));
    addAndMakeVisible(waveform.get());
    waveform->setVisible(false);
    
    loadingFilesProgress.reset(new juce::ProgressBar(loadingProgress));
    addAndMakeVisible(loadingFilesProgress.get());
    loadingFilesProgress->setVisible(false);
    
    analysisProgress.reset(new juce::ProgressBar(loadingProgress));
    addAndMakeVisible(analysisProgress.get());
    analysisProgress->setVisible(false);
    
    trackDataUpdate.store(false);
}

void LibraryComponent::resized()
{
    trackTable->setSize(getWidth(), getHeight() - 40);
    trackTable->setTopLeftPosition(0, 0);
    
    chooseFolderBtn->setSize(120, 40);
    chooseFolderBtn->setCentrePosition(getWidth()/2, getHeight()/2);
    
    loadingFilesProgress->setSize(200, 20);
    loadingFilesProgress->setCentrePosition(getWidth()/2, getHeight()/2);
    
    analysisProgress->setSize(200, 20);
    analysisProgress->setTopLeftPosition(10, getHeight() - 30);
    
    waveform->setSize(getWidth(), getHeight());
}


void LibraryComponent::buttonClicked(juce::Button* button)
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


void LibraryComponent::timerCallback()
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


void LibraryComponent::chooseFolder()
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


void LibraryComponent::loadFiles()
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
