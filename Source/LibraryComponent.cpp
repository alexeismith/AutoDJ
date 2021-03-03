//
//  LibraryComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/02/2021.
//

#include "LibraryComponent.hpp"


LibraryComponent::LibraryComponent()
{
    juce::FileChooser chooser ("Choose Music Folder");
    if (chooser.browseForDirectory())
    {
        dataManager.initialise(chooser.getResult());
    }
    
    fileList.reset(new juce::FileListComponent(dataManager.getContents()));
    addAndMakeVisible(fileList.get());
}

void LibraryComponent::resized()
{
    fileList->setSize(getWidth(), getHeight());
    fileList->setTopLeftPosition(0, 0);
}
