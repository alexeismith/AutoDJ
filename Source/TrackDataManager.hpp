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
      
    TrackDataManager();
      
    ~TrackDataManager() {}
    
    void initialise(juce::File directory);
    
    juce::Array<TrackData>* getTracks() { return &tracks; }
    
    void update(TrackData track);
    
    void fetchAudio(juce::String filename, juce::AudioBuffer<float>& buffer, bool sumToMono = false);
    
    bool isReady() { return ready.load(); }
      
private:
    
    void printTrackData(TrackData data);
    
    void parseFile(juce::File file);
    
    void addToDatabase(juce::File file, int hash, TrackData& trackData);
    
    bool hasFileChanged(juce::File file, int existingHash);
    
    int getHash(juce::File file);
    
    juce::AudioFormatManager formatManager;
    juce::WildcardFileFilter fileFilter;
    SqlDatabase database;
    
    juce::Array<TrackData> tracks;
    
    juce::TimeSliceThread thread {"BackgroundUpdateThread"};
    std::unique_ptr<juce::DirectoryContentsList> dirContents;
    
    std::atomic<bool> ready;
    
    friend class FileParserThread;
    
    std::unique_ptr<FileParserThread> parser;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackDataManager)
};


class FileParserThread : public juce::ThreadWithProgressWindow
{
public:
    
    FileParserThread(TrackDataManager* dm) :
        juce::ThreadWithProgressWindow("Loading Library", true, false), dataManager(dm) {}
    
    ~FileParserThread() {}
    
    void run();
    
private:
    
    TrackDataManager* dataManager;
};

#endif /* TrackDataManager_hpp */
