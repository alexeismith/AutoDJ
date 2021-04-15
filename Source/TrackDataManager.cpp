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
    fileFilter(juce::WildcardFileFilter("*.wav,*.mp3", "*", "AudioFormats"))
{
    formatManager.registerFormat(new juce::WavAudioFormat(), false);
    formatManager.registerFormat(new juce::MP3AudioFormat(), false);
    
    thread.startThread(3);
    dirContents.reset(new juce::DirectoryContentsList(&fileFilter, thread));
    
    parser.reset(new FileParserThread(this));
    
    ready.store(false);
}


void TrackDataManager::initialise(juce::File directory)
{
    dirContents->setDirectory(directory, true, true);
    
    if (!database.initialise(directory))
        jassert(false); // Database failed to initialise
    
    parser->startThread();
}


void TrackDataManager::update(TrackData track)
{
    database.store(track);
    for (int i = 0; i < tracks.size(); i++)
    {
        if (tracks[i].hash == track.hash)
        {
            tracks.remove(i);
            tracks.add(track);
            return;
        }
    }
}


void TrackDataManager::fetchAudio(juce::String filename, juce::AudioBuffer<float>& buffer, bool sumToMono)
{
    juce::String filePath = dirContents->getDirectory().getFullPathName() + "/" + filename;
    
    juce::AudioFormatReader* reader = formatManager.createReaderFor(filePath);
    
    if (reader)
    {
        buffer.setSize(reader->numChannels, (int)reader->lengthInSamples);
        
        reader->read(buffer.getArrayOfWritePointers(), reader->numChannels, 0, (int)reader->lengthInSamples);
        
        if (sumToMono && buffer.getNumChannels() == 2)
        {
            buffer.applyGain(0.5f);
            buffer.addFrom(0, 0, buffer.getReadPointer(1), buffer.getNumSamples());
            buffer.setSize(1, buffer.getNumSamples(), true);
        }
        
        delete reader;
    }
}


bool TrackDataManager::isReady(double& progress)
{
    progress = parser->getProgress();
    return ready.load();
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
    "\nBeat Phase: " << data.beatPhase << \
    "\nDownbeat: " << data.downbeat << \
    "\nKey: " << data.key << \
    "\nEnergy: " << data.energy << '\n';
    
    DBG(ss.str());
}


void TrackDataManager::parseFile(juce::File file)
{
    int hash;
    TrackData trackData;
    
    hash = getHash(file);
    
    trackData = database.read(file.getFileName());
    
    // If the file has changed since the data was stored, or the file did not exist in the database, replace the database entry with a new one
    // (If the existing hash is zero, the track hasn't been found in the database)
    if (hash != trackData.hash)
        addToDatabase(file, hash, trackData);
    
    tracks.add(trackData);
}


void TrackDataManager::addToDatabase(juce::File file, int hash, TrackData& trackData)
{
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
        
//        DBG("Added to database: " << trackData.filename);
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


void FileParserThread::run()
{
    int numFiles;
    
    while (dataManager->dirContents->isStillLoading())
    {
        _mm_pause();
    }
    
    numFiles = dataManager->dirContents->getNumFiles();
    DBG("Num valid files in directory: " << numFiles);
    
    for (int i = 0; i < numFiles; i++)
    {
        progress.store(double(i) / numFiles);
        dataManager->parseFile(dataManager->dirContents->getFile(i));
    }
    
    dataManager->ready.store(true);
}
