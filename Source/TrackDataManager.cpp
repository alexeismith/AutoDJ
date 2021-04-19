//
//  TrackDataManager.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 01/03/2021.
//

#include "TrackDataManager.hpp"
#include "CommonDefs.hpp"
#include "ThirdParty/xxhash32.h"

#include "LibraryComponent.hpp"

extern "C" {
  #include <sqlite3.h>
}


TrackDataManager::TrackDataManager(juce::Component* library) :
    libraryComponent(library),
    fileFilter(juce::WildcardFileFilter("*.wav,*.mp3", "*", "AudioFormats"))
{
    formatManager.registerFormat(new juce::WavAudioFormat(), false);
    formatManager.registerFormat(new juce::MP3AudioFormat(), false);
    
    thread.startThread(3);
    dirContents.reset(new juce::DirectoryContentsList(&fileFilter, thread));
    
    parser.reset(new FileParserThread(this));
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
    const juce::ScopedLock sl (lock);
    
    database.store(track);
    
    for (int i = 0; i < tracks.size(); i++)
    {
        if (tracks[i].hash == track.hash)
        {
            tracks.remove(i);
            tracks.add(track);
            break;
        }
    }
    
    ((LibraryComponent*)libraryComponent)->updateData();
}


void TrackDataManager::fetchAudio(juce::String filename, juce::AudioBuffer<float>& buffer, bool sumToMono)
{
    const juce::ScopedLock sl (lock);
    
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


bool TrackDataManager::isLoaded(double& progress)
{
    progress = parser->getProgress();
    return (progress > 1.0);
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
    TrackData trackData, existingData;
    
    trackData.hash = getHash(file);
    
    existingData = database.read(file.getFileName());
    
    // Fetches metadata from file and returns whether it is valid
    if (getTrackData(file, trackData))
    {
        // If the file has changed since the data was stored, or the file did not exist in the database, replace the database entry with a new one
        // (If the existing hash is zero, the track hasn't been found in the database)
        if (trackData.hash != existingData.hash)
        {
            database.store(trackData);
        }
        else
        {
            trackData = existingData;
        }
        
        tracks.add(trackData);
    }
}


bool TrackDataManager::getTrackData(juce::File file, TrackData& trackData)
{
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));

    if (reader)
    {
        trackData.filename = file.getFileName();
        trackData.artist = reader->metadataValues.getValue("IART", "");
        trackData.title = reader->metadataValues.getValue("INAM", "");
        trackData.length = round(reader->lengthInSamples / reader->sampleRate);

        if (reader->numChannels > 0 && trackData.length >= TRACK_LENGTH_SECS_MIN && trackData.length <= TRACK_LENGTH_SECS_MAX)
            return true;
    }
    
    DBG("Not valid: " << file.getFileName());
    return false;
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
    DBG("Num files in directory: " << numFiles);
    
    for (int i = 0; i < numFiles; i++)
    {
        if (threadShouldExit()) return;
        progress.store(double(i) / numFiles);
        dataManager->parseFile(dataManager->dirContents->getFile(i));
    }
    
    progress.store(2.0);
}
