//
//  TrackDataManager.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 01/03/2021.
//

#ifndef DataManager_hpp
#define DataManager_hpp

#include <JuceHeader.h>

#include "SqlDatabase.hpp"
#include "TrackSorter.hpp"
#include "AnalysisManager.hpp"

#define DATABASE_FILENAME ("AutoDjData.db")

class FileParserThread;

class TrackDataManager
{
public:
      
    TrackDataManager();
      
    ~TrackDataManager();
    
    void initialise(juce::Component* library, juce::File directory);
    
    TrackInfo* getTracks() { return tracks; }
    
    int getNumTracks() { return numTracks; }
    
    int getNumTracksReady() { return numTracksAnalysedUnplayed; }
    
    void storeAnalysis(TrackInfo* track);
    
    void trackQueued() { numTracksAnalysedUnplayed -= 1; }
    
    bool isLoaded(double& progress);
    
    bool analysisProgress(double& progress, bool& canStartPlaying);
    
    juce::AudioBuffer<float>* loadAudio(juce::String filename, bool mono = false);
    
    void releaseAudio(juce::AudioBuffer<float>* buffer) { const juce::ScopedLock sl(lock); audioBuffers.removeObject(buffer); }
    
    TrackSorter* getSorter() { return &sorter; }
      
private:
    
    void adjustChannels(juce::AudioBuffer<float>* buffer, bool mono);
    
    void printTrackInfo(TrackInfo data);
    
    void parseFile(juce::File file);
    
    // Returns whether the track is valid
    bool getTrackInfo(juce::File file, TrackInfo& trackInfo);
    
    int getHash(juce::File file);
    
    juce::OwnedArray<juce::AudioBuffer<float>> audioBuffers;
    
    juce::Component* libraryComponent;
    
    std::unique_ptr<AnalysisManager> analysisManager;
    
    juce::AudioFormatManager formatManager;
    juce::WildcardFileFilter fileFilter;
    SqlDatabase database;
    
    TrackSorter sorter;
    
    TrackInfo* tracks;
    int numTracks = 0;
    int numTracksAnalysed = 0;
    int numTracksAnalysedUnplayed = 0;
    
    juce::TimeSliceThread thread {"BackgroundUpdateThread"};
    std::unique_ptr<juce::DirectoryContentsList> dirContents;
    
    juce::CriticalSection lock;
    
    friend class FileParserThread;
    
    std::unique_ptr<FileParserThread> parser;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackDataManager)
};


class FileParserThread : public juce::Thread
{
public:
    
    FileParserThread(TrackDataManager* dm) :
        juce::Thread("FileParser"), dataManager(dm) {}
    
    ~FileParserThread() { stopThread(10000); }
    
    void run();
    
    double getProgress() { return progress.load(); }
    
private:
    
    TrackDataManager* dataManager;
    std::atomic<double> progress;
    
};

#endif /* TrackDataManager_hpp */
