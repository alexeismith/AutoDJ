//
//  LibraryComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/02/2021.
//

#include "LibraryComponent.hpp"


LibraryComponent::LibraryComponent(AudioProcessor* p)
{
    audioProcessor = p;
    
    analysisManager.reset(new AnalysisManager(&dataManager));
    
    trackTable.reset(new QueueTableComponent());
    addAndMakeVisible(trackTable.get());
    trackTable->setVisible(false);
    trackTable->addColumns();
    
    chooseFolderBtn.reset(new juce::TextButton("Choose Folder"));
    addAndMakeVisible(chooseFolderBtn.get());
    chooseFolderBtn->setSize(120, 40);
    chooseFolderBtn->addListener(this);
    
    waveform.reset(new WaveformComponent(800, &dataManager));
    addAndMakeVisible(waveform.get());
    waveform->setVisible(false);
}

void LibraryComponent::resized()
{
    trackTable->setSize(getWidth(), getHeight());
    trackTable->setTopLeftPosition(0, 0);
    
    chooseFolderBtn->setCentrePosition(getWidth()/2, getHeight()/2);
    
    waveform->setSize(getWidth(), getHeight());
}


void LibraryComponent::buttonClicked(juce::Button* button)
{
    int id = button->getComponentID().getIntValue();
    
    switch (id)
    {
        case ComponentIDs::chooseFolder:
            chooseFolder();
            break;
        default:
            jassert(false); // Unrecognised button ID
    }
}

void LibraryComponent::chooseFolder()
{
    juce::FileChooser chooser ("Choose Music Folder");
    if (chooser.browseForDirectory())
    {
        dataManager.initialise(chooser.getResult());
        
        TrackData track = dataManager.getTracks()->getReference(6);
        
//        analysisManager->analyse(track);
        
        trackTable->populate(dataManager.getTracks());
        
        trackTable->setVisible(true);
        chooseFolderBtn->setVisible(false);
        
        DBG("Waveform");
        waveform->loadTrack(track, -80000);
        DBG("Waveform done");
        waveform->setVisible(true);
        
//        juce::AudioBuffer<float> buffer;
//        dataManager.fetchAudio(track.filename, buffer, true);
//        buffer.copyFrom(0, 0, buffer.getReadPointer(0, 540000), 801*WAVEFORM_FRAME_SIZE);
//        buffer.setSize(1, 800*WAVEFORM_FRAME_SIZE, true);
//        audioProcessor->play(buffer);
    }
}
