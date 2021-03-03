//
//  LibraryComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/02/2021.
//

#include "LibraryComponent.hpp"


LibraryComponent::LibraryComponent()
{
    thread.startThread(3);
    
    dirContents.reset(new juce::DirectoryContentsList(nullptr, thread));
    
    juce::FileChooser chooser ("Choose Music Folder");
    if (chooser.browseForDirectory())
    {
        dirContents->setDirectory(chooser.getResult(), true, true);
        dataManager.initialise(chooser.getResult());
    }
    
    fileList.reset(new juce::FileListComponent(*dirContents));
    addAndMakeVisible(fileList.get());
}

void LibraryComponent::resized()
{
    fileList->setSize(getWidth(), getHeight());
    fileList->setTopLeftPosition(0, 0);
}
