//
//  TrackDataManager.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 01/03/2021.
//

#include "TrackDataManager.hpp"
#include "CommonDefs.hpp"
#include "ThirdParty/xxhash32.h"

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
    "\nHash: " << data.hash << \
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
    juce::File file;
    int hash;
    TrackData existingData;
    
    DBG("Num WAV files in directory: " << dirContents->getNumFiles());
    
    for (int i = 0; i < dirContents->getNumFiles(); i++)
    {
        file = dirContents->getFile(i);
        hash = getHash(file);
        
        existingData = database.read(file.getFileName());
        
        // If the existing hash is zero, the track hasn't been found in the database
        if (existingData.hash == 0)
            addToDatabase(file, hash);
        // Otherwise, if the file has changed since the data was stored, replace the database entry with a new one
        else if (hash != existingData.hash)
            addToDatabase(file, hash);
    }
    // TODO: remove rows for files that are no longer present
}


void TrackDataManager::addToDatabase(juce::File file, int hash)
{
    TrackData trackData;
    trackData.filename = file.getFileName();
    trackData.hash = hash;
    
    juce::AudioFormatReader* reader = formatManager.createReaderFor(file);

    if (reader)
    {
        trackData.artist = reader->metadataValues.getValue("IART", "");
        trackData.title = reader->metadataValues.getValue("INAM", "");
        
        trackData.length = round(reader->lengthInSamples / reader->sampleRate);
        
        if (trackData.length >= TRACK_LENGTH_SECS_MIN && trackData.length <= TRACK_LENGTH_SECS_MAX)
            database.store(trackData);

        delete reader;
        
        DBG("Added to database: " << trackData.filename);
    }
}


bool TrackDataManager::hasFileChanged(juce::File file, int existingHash)
{
    juce::MemoryBlock rawFile;
    int newHash;

    file.loadFileAsData(rawFile);
    newHash = XXHash32::hash(rawFile.getData(), rawFile.getSize(), 0);
    rawFile.reset();
    
    return (newHash == existingHash);
}


int TrackDataManager::getHash(juce::File file)
{
    juce::MemoryBlock rawFile;
    int hash;

    file.loadFileAsData(rawFile);
    hash = XXHash32::hash(rawFile.getData(), rawFile.getSize(), 0);
    rawFile.reset();
    
    return hash;
}
