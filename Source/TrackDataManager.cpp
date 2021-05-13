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
    
    analysisManager.reset(new AnalysisTest());
    
    parser.reset(new FileParserThread(this));
    
    reset();
}


TrackDataManager::~TrackDataManager()
{
    // Delete file parser first, because it might try to access analysisManager before it dies
    parser.reset();
    // Then delete analysisManager, because it might try to access dirContents before it dies
    analysisManager.reset();
    dirContents.reset();
}


bool TrackDataManager::initialise(juce::File directory, DirectionView* direction)
{
    directionView = direction;
    
    dirContents->setDirectory(directory, true, true);
    
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


void TrackDataManager::storeAnalysis(TrackInfo* track)
{
    const juce::ScopedLock sl(lock);
    
    // Update the database with the new track info (this replaces the existing track record if present)
    database.store(*track);

    // Pass the track to the sorter
    sorter.addAnalysed(track);
    
    // Pass the track to the direction view
    directionView->addAnalysed(track);
    
    // Increment the counters
    numTracksAnalysed += 1;
    numTracksAnalysedUnqueued += 1;
    
    trackDataUpdate.store(true);
}


bool TrackDataManager::isLoading(double& progress, bool& valid)
{
    progress = parser->getProgress();
    
    valid = validDirectory.load();
    
    return (parser->isThreadRunning());
}


bool TrackDataManager::analysisProgress(double& progress, bool& canStartPlaying)
{
    canStartPlaying = numTracksAnalysed >= NUM_TRACKS_MIN;
    return analysisManager->isFinished(progress);
}


juce::AudioBuffer<float>* TrackDataManager::loadAudio(juce::String filename, bool mono)
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


void TrackDataManager::adjustChannels(juce::AudioBuffer<float>* buffer, bool mono)
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
    "\nGroove: " << info.groove << '\n';
    
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
            analysisManager->processResult(trackPtr);
            sorter.addAnalysed(trackPtr);
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


void TrackDataManager::clearHistory()
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
            
            sorter.addAnalysed(track);
            
            // Pass the track to the direction view
            directionView->addAnalysed(track);
        }
    }
    
    trackDataUpdate.store(true);
}


void TrackDataManager::reset()
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
    TrackDataManager* dm = dataManager;
    
    while (dm->dirContents->isStillLoading())
    {
        _mm_pause();
    }
    
    numFiles = dm->dirContents->getNumFiles();
    DBG("Num files in directory: " << numFiles);
    
    dm->tracks = (TrackInfo*)malloc(sizeof(TrackInfo) * numFiles);
    
    for (int i = 0; i < numFiles; i++)
    {
        if (threadShouldExit()) return;
        progress.store(double(i) / numFiles);
        dm->parseFile(dm->dirContents->getFile(i));
    }
    
    if (dm->numTracks < NUM_TRACKS_MIN)
    {
        dm->analysisManager->clearJobs();
        dm->reset();
        
        memset(dm->tracks, 0, dm->numTracksAnalysed * sizeof(TrackInfo));
        return;
    }
    
    dm->validDirectory.store(true);
    
    DBG("Num already analysed: " << dm->numTracksAnalysed);
    
    dm->analysisManager->startAnalysis(dm);
}
