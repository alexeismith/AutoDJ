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
#include "AnalysisTest.hpp"
#include "DirectionView.hpp"

class FileParserThread;

class DataManager
{
public:
      
    DataManager();
      
    ~DataManager();
    
    bool initialise(juce::File directory, DirectionView* directionView);
    
    TrackInfo* getTracks() { return tracks; }
    
    int getNumTracks() { return numTracks; }
    
    int getNumTracksReady() { return numTracksAnalysedUnqueued; }
    
    void storeAnalysis(TrackInfo* track);
    
    void trackQueued() { numTracksAnalysedUnqueued -= 1; }
    
    bool isLoading(double& progress, bool& validDirectory);
    
    bool analysisProgress(double& progress, bool& canStartPlaying);
    
    juce::AudioBuffer<float>* loadAudio(juce::String filename, bool mono = false);
    
    void releaseAudio(juce::AudioBuffer<float>* buffer) { const juce::ScopedLock sl(lock); audioBuffers.removeObject(buffer); }
    
    TrackSorter* getSorter() { return &sorter; }
    
    AnalysisResults getAnalysisResults() { return analysisManager->getResults(); }
    
    AnalysisManager* getAnalysisManager() { return analysisManager.get(); }
    
    void clearHistory();
    
    std::atomic<bool> trackDataUpdate = false;
      
private:
    
    void adjustChannels(juce::AudioBuffer<float>* buffer, bool mono);
    
    void printTrackInfo(TrackInfo data);
    
    void parseFile(juce::File file);
    
    // Returns whether the track is valid
    bool getTrackInfo(juce::File file, TrackInfo& trackInfo);
    
    int getHash(juce::File file);
    
    void reset();
    
    juce::OwnedArray<juce::AudioBuffer<float>> audioBuffers;
    
    std::unique_ptr<AnalysisManager> analysisManager;
    
    juce::AudioFormatManager formatManager;
    juce::WildcardFileFilter fileFilter;
    SqlDatabase database;
    
    TrackSorter sorter;
    
    TrackInfo* tracks;
    int numTracks;
    int numTracksAnalysed;
    int numTracksAnalysedUnqueued;
    
    juce::TimeSliceThread thread {"BackgroundUpdateThread"};
    std::unique_ptr<juce::DirectoryContentsList> dirContents;
    
    juce::CriticalSection lock;
    
    friend class FileParserThread;
    
    std::unique_ptr<FileParserThread> parser;
    
    std::atomic<bool> initialised = false;
    
    std::atomic<bool> validDirectory = false;
    
    DirectionView* directionView;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DataManager)
};


class FileParserThread : public juce::Thread
{
public:
    
    FileParserThread(DataManager* dm) :
        juce::Thread("FileParser"), dataManager(dm) {}
    
    ~FileParserThread() { stopThread(10000); }
    
    void run();
    
    double getProgress() { return progress.load(); }
    
private:
    
    DataManager* dataManager;
    std::atomic<double> progress = 0.0;
    
};

#endif /* TrackDataManager_hpp */
