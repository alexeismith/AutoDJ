//
//  LibraryComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/02/2021.
//

#include "LibraryComponent.hpp"


LibraryComponent::LibraryComponent()
{
    analysisManager.reset(new AnalysisManager(&dataManager));
    
    trackTable.reset(new QueueTableComponent());
    addAndMakeVisible(trackTable.get());
    trackTable->setVisible(false);
    trackTable->addColumns();
    
    chooseFolderBtn.reset(new juce::TextButton("Choose Folder"));
    addAndMakeVisible(chooseFolderBtn.get());
    chooseFolderBtn->setSize(120, 40);
    chooseFolderBtn->addListener(this);
}

void LibraryComponent::resized()
{
    trackTable->setSize(getWidth(), getHeight());
    trackTable->setTopLeftPosition(0, 0);
    
    chooseFolderBtn->setCentrePosition(getWidth()/2, getHeight()/2);
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
        
        trackTable->populate(dataManager.getTracks());
        
        trackTable->setVisible(true);
        chooseFolderBtn->setVisible(false);
        
        analysisManager->analyse(*dataManager.getTracks()->begin());
    }
}
