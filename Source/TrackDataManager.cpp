//
//  TrackDataManager.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 01/03/2021.
//

#include "TrackDataManager.hpp"

extern "C" {
  #include <sqlite3.h>
}


TrackDataManager::TrackDataManager() :
    fileFilter(juce::WildcardFileFilter("*.wav,*.mp3,*.aiff,*.m4a", "*", "AudioFormats"))
{
    formatManager.registerBasicFormats();
    
    thread.startThread(3);
    dirContents.reset(new juce::DirectoryContentsList(&fileFilter, thread));
}


void TrackDataManager::initialise(juce::File directory)
{
    dirContents->setDirectory(directory, true, true);
    
    if (!database.initialise(directory))
        jassert(false); // Database failed to initialise
    
    parseFiles();
    
    database.store(TrackData {"Example.mp3", "The Beatles", "Noise, Waves & Fields", 7938000, 130, 11, 3});
    
    printTrackData(database.read("Example.mp3"));
}


void TrackDataManager::printTrackData(TrackData data)
{
    std::stringstream ss;
    ss << "\nTrack Data..." << \
    "\nFilename: " << data.filename << \
    "\nArtist: " << data.artist << \
    "\nTitle: " << data.title << \
    "\nLength: " << data.length << \
    "\nBPM: " << data.bpm << \
    "\nKey: " << data.key << \
    "\nEnergy: " << data.energy << '\n';
    
    DBG(ss.str());
}


void TrackDataManager::parseFiles()
{
    DBG("NUM FILES: " << dirContents->getNumFiles());
}
