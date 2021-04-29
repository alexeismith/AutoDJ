//
//  TrackDataManager.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 01/03/2021.
//

#include "TrackDataManager.hpp"

#include "CommonDefs.hpp"
#include "LibraryView.hpp"

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
    
    analysisManager.reset(new AnalysisManager());
    
    parser.reset(new FileParserThread(this));
}


TrackDataManager::~TrackDataManager()
{
    // Must delete analysisManager before dirContents because the analysis threads might try to access dirContents
    analysisManager.reset();
    dirContents.reset();
}


void TrackDataManager::initialise(juce::Component* library, juce::File directory)
{
    libraryComponent = library;
    
    dirContents->setDirectory(directory, true, true);
    
    if (!database.initialise(directory))
        jassert(false); // Database failed to initialise
    
    parser->startThread();
}


void TrackDataManager::storeAnalysis(TrackInfo* track)
{
    const juce::ScopedLock sl(lock);
    
    // Update the database with the new track info (this replaces the existing track record if present)
    database.store(*track);

    // Pass the track to the sorter
    sorter.addAnalysed(track);
    // Increment the counters
    numTracksAnalysed += 1;
    numTracksAnalysedUnplayed += 1;
    
    // If the library UI is set, tell it to update
    if (libraryComponent)
        ((LibraryView*)libraryComponent)->updateData();
}


void TrackDataManager::markTrackQueued(TrackInfo* track)
{
    sorter.remove(track);
    numTracksAnalysedUnplayed -= 1;
}


bool TrackDataManager::isLoaded(double& progress)
{
    progress = parser->getProgress();
    return (!parser->isThreadRunning());
}


bool TrackDataManager::analysisProgress(double& progress, bool& canStartPlaying)
{
    canStartPlaying = numTracksAnalysed >= NUM_TRACKS_MIN;
    return analysisManager->isFinished(progress);
}


juce::AudioBuffer<float>* TrackDataManager::loadAudio(juce::String filename, bool mono)
{
    const juce::ScopedLock sl(lock);
    
//    DBG("LOADING AUDIO");
    
    juce::AudioBuffer<float>* buffer = nullptr;
    
    juce::String filePath = dirContents->getDirectory().getFullPathName() + "/" + filename;
    
    juce::AudioFormatReader* reader = formatManager.createReaderFor(filePath);
    
    if (reader)
    {
        buffer = new juce::AudioBuffer<float>();
        audioBuffers.add(buffer);
        
        buffer->clear();
        buffer->setSize(reader->numChannels, (int)reader->lengthInSamples);
        
        reader->read(buffer->getArrayOfWritePointers(), reader->numChannels, 0, (int)reader->lengthInSamples);
        
        adjustChannels(buffer, mono);
        
        delete reader;
    }
    else
    {
        jassert(false); // Failed to load track audio
        // TODO: handle the case where files are deleted after parsing
    }
    
    return buffer;
}


void TrackDataManager::adjustChannels(juce::AudioBuffer<float>* buffer, bool mono)
{
    if (mono && buffer->getNumChannels() == 2)
    {
        buffer->applyGain(0.5f);
        buffer->addFrom(0, 0, buffer->getReadPointer(1), buffer->getNumSamples());
        buffer->setSize(1, buffer->getNumSamples(), true);
    }
    else if (buffer->getNumChannels() == 1)
    {
        buffer->setSize(2, buffer->getNumSamples(), true);
        buffer->addFrom(1, 0, buffer->getReadPointer(0), buffer->getNumSamples());
    }
    else if (buffer->getNumChannels() > 2)
    {
        buffer->setSize(2, buffer->getNumSamples(), true);
    }
}


void TrackDataManager::printTrackInfo(TrackInfo info)
{
    std::stringstream ss;
    ss << "\nTrack Data..." << \
    "\nFilename: " << info.getFilename() << \
    "\nHash: " << info.hash << \
    "\nArtist: " << info.getArtist() << \
    "\nTitle: " << info.getTitle() << \
    "\nLength: " << info.length << \
    "\nAnalysed: " << info.analysed << \
    "\nBPM: " << info.bpm << \
    "\nBeat Phase: " << info.beatPhase << \
    "\nDownbeat: " << info.downbeat << \
    "\nKey: " << info.key << \
    "\nEnergy: " << info.energy << '\n';
    
    DBG(ss.str());
}


void TrackDataManager::parseFile(juce::File file)
{
    TrackInfo trackInfo, existingInfo;
    
    trackInfo.hash = getHash(file);
    
    existingInfo = database.read(file.getFileName());
    
    // Fetches metadata from file and returns whether it is valid
    if (getTrackInfo(file, trackInfo))
    {
        // If the file has changed since the data was stored, or the file did not exist in the database, replace the database entry with a new one
        // (If the existing hash is zero, the track hasn't been found in the database)
        if (trackInfo.hash != existingInfo.hash)
        {
            database.store(trackInfo);
        }
        else
        {
            trackInfo = existingInfo;
        }
        
        tracks[numTracks] = trackInfo;
        TrackInfo* trackPtr = &tracks[numTracks];
        numTracks += 1;
        
        if (trackInfo.analysed)
        {
            sorter.addAnalysed(trackPtr);
            numTracksAnalysed += 1;
            numTracksAnalysedUnplayed += 1;
        }
        else
        {
            analysisManager->addJob(trackPtr);
        }
    }
}


bool TrackDataManager::getTrackInfo(juce::File file, TrackInfo& trackInfo)
{
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));

    if (reader)
    {
        trackInfo.setFilename(file.getFileName());
        trackInfo.setArtist(reader->metadataValues.getValue("IART", ""));
        trackInfo.setTitle(reader->metadataValues.getValue("INAM", ""));
        trackInfo.length = round(reader->lengthInSamples / reader->sampleRate);

        if (reader->numChannels > 0 && trackInfo.length >= TRACK_LENGTH_SECS_MIN && trackInfo.length <= TRACK_LENGTH_SECS_MAX)
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
    
    dataManager->tracks = (TrackInfo*)malloc(sizeof(TrackInfo) * numFiles);
    
    for (int i = 0; i < numFiles; i++)
    {
        if (threadShouldExit()) return;
        progress.store(double(i) / numFiles);
        dataManager->parseFile(dataManager->dirContents->getFile(i));
    }
    
    DBG("Num already analysed: " << dataManager->numTracksAnalysed);
    
    dataManager->sorter.sort();
//    dataManager->analysisManager->startAnalysis(dataManager);
}
