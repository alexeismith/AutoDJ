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

#define DATABASE_FILENAME ("AutoDjData.db")

class FileParserThread;

class TrackDataManager
{
public:
      
    TrackDataManager(juce::Component* library);
      
    ~TrackDataManager() {}
    
    void initialise(juce::File directory);
    
    juce::Array<TrackData>* getTracks() { return &tracks; }
    
    void update(TrackData track);
    
    void fetchAudio(juce::String filename, juce::AudioBuffer<float>& buffer, bool sumToMono = false);
    
    bool isLoaded(double& progress);
      
private:
    
    void printTrackData(TrackData data);
    
    void parseFile(juce::File file);
    
    // Returns whether the track is valid
    bool getTrackData(juce::File file, TrackData& trackData);
    
    int getHash(juce::File file);
    
    juce::Component* libraryComponent;
    
    juce::AudioFormatManager formatManager;
    juce::WildcardFileFilter fileFilter;
    SqlDatabase database;
    
    juce::Array<TrackData> tracks;
    
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
        juce::Thread("Parser"), dataManager(dm) {}
    
    ~FileParserThread() { stopThread(1000); }
    
    void run();
    
    double getProgress() { return progress.load(); }
    
private:
    
    TrackDataManager* dataManager;
    std::atomic<double> progress;
    
};

#endif /* TrackDataManager_hpp */
