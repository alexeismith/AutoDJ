//
//  LibraryComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/02/2021.
//

#include "LibraryComponent.hpp"


LibraryComponent::LibraryComponent()
{
    fileList.reset(new juce::FileListComponent(dataManager.getContents()));
    addAndMakeVisible(fileList.get());
    fileList->setVisible(false);
    
    chooseFolderBtn.reset(new juce::TextButton("Choose Folder"));
    addAndMakeVisible(chooseFolderBtn.get());
    chooseFolderBtn->setSize(120, 40);
    chooseFolderBtn->addListener(this);
}

void LibraryComponent::resized()
{
    fileList->setSize(getWidth(), getHeight());
    fileList->setTopLeftPosition(0, 0);
    
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
    }
    
    fileList->setVisible(true);
    chooseFolderBtn->setVisible(false);
}
