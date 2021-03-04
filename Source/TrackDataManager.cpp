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
    fileFilter(juce::WildcardFileFilter("*.wav", "*", "AudioFormats"))
{
    formatManager.registerFormat(new juce::WavAudioFormat(), false);
    formatManager.registerFormat(new juce::MP3AudioFormat(), false);
    
    thread.startThread(3);
    dirContents.reset(new juce::DirectoryContentsList(&fileFilter, thread));
}


void TrackDataManager::initialise(juce::File directory)
{
    dirContents->setDirectory(directory, true, true);
    
    if (!database.initialise(directory))
        jassert(false); // Database failed to initialise
    
    parseFiles();
    
    printTrackData(database.read("Vesta.wav"));
    printTrackData(database.read("Cool Today (New Ends Remix).wav"));
}


void TrackDataManager::printTrackData(TrackData data)
{
    std::stringstream ss;
    ss << "\nTrack Data..." << \
    "\nFilename: " << data.filename << \
    "\nArtist: " << data.artist << \
    "\nTitle: " << data.title << \
    "\nLength: " << data.length << \
    "\nAnalysed: " << data.analysed << \
    "\nBPM: " << data.bpm << \
    "\nKey: " << data.key << \
    "\nEnergy: " << data.energy << '\n';
    
    DBG(ss.str());
}


void TrackDataManager::parseFiles()
{
    TrackData trackData;
    
    DBG("NUM FILES: " << dirContents->getNumFiles());
    
    for (int i = 0; i < dirContents->getNumFiles(); i++)
    {
        trackData = getTrackData(dirContents->getFile(i));
        database.store(trackData);
    }
    
    // TODO: Check MD5 hash
    // TODO: remove rows for files that are no longer present
}


TrackData TrackDataManager::getTrackData(juce::File file)
{
    TrackData trackData;
    
    juce::AudioFormatReader* reader = formatManager.createReaderFor(file);
    
    trackData.filename = file.getFileName();

    if (reader)
    {
        trackData.artist = reader->metadataValues.getValue("IART", "");
        trackData.title = reader->metadataValues.getValue("INAM", "");
        
        trackData.length = round(reader->lengthInSamples / reader->sampleRate);

        delete reader;
    }
    
    return trackData;
}
