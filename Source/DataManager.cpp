//
//  TrackDataManager.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 01/03/2021.
//

#include "DataManager.hpp"

#include "CommonDefs.hpp"

#include "ThirdParty/xxhash32.h"

extern "C" {
  #include <sqlite3.h>
}


DataManager::DataManager() :
    fileFilter(juce::WildcardFileFilter("*.wav,*.mp3", "*", "AudioFormats"))
{
    formatManager.registerFormat(new juce::WavAudioFormat(), false);
    formatManager.registerFormat(new juce::MP3AudioFormat(), false);
    
    thread.startThread(3);
    dirContents.reset(new juce::DirectoryContentsList(&fileFilter, thread));
    
    analysisManager.reset(new AnalysisManager());
    
    parser.reset(new FileParserThread(this));
    
    reset();
}


DataManager::~DataManager()
{
    // Delete file parser first, because it might try to access analysisManager before it dies
    parser.reset();
    // Then delete analysisManager, because it might try to access dirContents before it dies
    analysisManager.reset();
    dirContents.reset();
}


bool DataManager::initialise(juce::File directory, DirectionView* direction)
{
    directionView = direction;
    
    dirContents->setDirectory(directory, true, true);
    
    dirContents->refresh();
    
    if (!database.initialise(directory))
    {
        // Show an error window
        juce::AlertWindow::showMessageBox(juce::AlertWindow::WarningIcon, "Error", "Failed to initialise database.", "OK");
        jassert(false); // Database failed to initialise
        return false;
    }
    
    parser->startThread();
    
    initialised.store(true);
    
    return true;
}


void DataManager::storeAnalysis(TrackInfo* track)
{
    const juce::ScopedLock sl(lock);
    
    // Update the database with the new track info (this replaces the existing track record if present)
    database.store(*track);

    // Pass the track to the sorter
    sorter.addTrack(track);
    
    // Pass the track to the direction view
    directionView->addAnalysed(track);
    
    // Increment the counters
    numTracksAnalysed += 1;
    numTracksAnalysedUnqueued += 1;
    
    trackDataUpdate.store(true);
}


bool DataManager::isLoading(double& progress)
{
    progress = parser->getProgress();
    return (parser->isThreadRunning());
}


bool DataManager::isAnalysisFinished(double& progress)
{
    return analysisManager->isFinished(progress);
}


bool DataManager::isAnalysisFinished()
{
    double progressTemp;
    return analysisManager->isFinished(progressTemp);
}


bool DataManager::canStartPlaying()
{
    if (validDirectory.load() && numTracksAnalysed >= NUM_TRACKS_MIN)
        return true;
    
    return false;
}


juce::AudioBuffer<float>* DataManager::loadAudio(juce::String filename, bool mono)
{
    juce::AudioBuffer<float>* buffer;
    juce::AudioFormatReader* reader = nullptr;
    juce::String filePath;
    
    {
        const juce::ScopedLock sl(lock);
        juce::String filePath = dirContents->getDirectory().getFullPathName() + "/" + filename;
        reader = formatManager.createReaderFor(filePath);
        
        if (reader)
        {
            audioBuffers.add(new juce::AudioBuffer<float>());
            buffer = audioBuffers.getLast();
        }
        else
        {
            jassert(false); // Failed to load track audio
            return nullptr;
            // TODO: handle the case where an audio file has been deleted when this request arrives
        }
    }
        
    buffer->clear();
    buffer->setSize(reader->numChannels, (int)reader->lengthInSamples);
    
    reader->read(buffer->getArrayOfWritePointers(), reader->numChannels, 0, (int)reader->lengthInSamples);
    
    adjustChannels(buffer, mono);
    
    delete reader;
    
    return buffer;
}


void DataManager::adjustChannels(juce::AudioBuffer<float>* buffer, bool mono)
{
    if (mono && buffer->getNumChannels() >= 2)
    {
        buffer->applyGain(0.5f);
        buffer->addFrom(0, 0, buffer->getReadPointer(1), buffer->getNumSamples());
        buffer->setSize(1, buffer->getNumSamples(), true);
    }
    else if (!mono && buffer->getNumChannels() == 1)
    {
        buffer->setSize(2, buffer->getNumSamples(), true);
        buffer->copyFrom(1, 0, buffer->getReadPointer(0), buffer->getNumSamples());
    }
    else if (buffer->getNumChannels() > 2)
    {
        buffer->setSize(2, buffer->getNumSamples(), true);
    }
}


void DataManager::printTrackInfo(TrackInfo info)
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
    "\nGroove: " << info.groove << '\n';
    
    DBG(ss.str());
}


void DataManager::parseFile(juce::File file)
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
            analysisManager->processResult(trackPtr);
            sorter.addTrack(trackPtr);
            directionView->addAnalysed(trackPtr);
            
            trackDataUpdate.store(true);
            
            numTracksAnalysed += 1;
            numTracksAnalysedUnqueued += 1;
        }
        else
        {
            analysisManager->addJob(trackPtr);
        }
    }
}


bool DataManager::getTrackInfo(juce::File file, TrackInfo& trackInfo)
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


int DataManager::getHash(juce::File file)
{
    juce::MemoryBlock rawFile;
    int hash;

    file.loadFileAsData(rawFile);
    hash = XXHash32::hash(rawFile.getData(), rawFile.getSize(), 0);
    rawFile.reset();
    
    return hash;
}


void DataManager::clearHistory()
{
    const juce::ScopedLock sl(lock);
    
    TrackInfo* track;
    
    sorter.reset();
    directionView->reset();
    
    numTracksAnalysed = 0;
    numTracksAnalysedUnqueued = 0;
    
    for (int i = 0; i < numTracks; i++)
    {
        track = &tracks[i];
        
        track->playing = false;
        track->played = false;
        
        if (track->analysed)
        {
            numTracksAnalysed += 1;
            numTracksAnalysedUnqueued += 1;
            
            analysisManager->processResult(track);
            
            sorter.addTrack(track);
            
            // Pass the track to the direction view
            directionView->addAnalysed(track);
        }
    }
    
    trackDataUpdate.store(true);
}


void DataManager::reset()
{
    numTracks = 0;
    numTracksAnalysed = 0;
    numTracksAnalysedUnqueued = 0;
    
    initialised.store(false);
    validDirectory.store(false);
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
    
    // Now that we know how many valid tracks there are,
    // if there aren't enough, reset and return
    if (dataManager->numTracks < NUM_TRACKS_MIN)
    {
        dataManager->analysisManager->clearJobs();
        dataManager->reset();
        
        memset(dataManager->tracks, 0, dataManager->numTracksAnalysed * sizeof(TrackInfo));
        return;
    }
    
    dataManager->validDirectory.store(true);
    
    DBG("Num already analysed: " << dataManager->numTracksAnalysed);
    
    dataManager->analysisManager->startAnalysis(dataManager);
}
