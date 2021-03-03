//
//  DataManager.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 01/03/2021.
//

#ifndef DataManager_hpp
#define DataManager_hpp

#include <JuceHeader.h>

#define DATABASE_FILENAME ("AutoDjData.db")

typedef struct TrackData {
    juce::String filename;
    juce::String artist;
    juce::String title;
    int length; // Total number of samples, divide this by SUPPORTED_SAMPLERATE to get length in seconds
    int bpm = -1; // Tempo in beats per minute
    int key = -1; // Musical key signature TODO: how to represent camelot?
    int energy = -1; // Overall energy level TODO: how to represent energy? thinking about track distribution screen, maybe a numeric scale is best
} TrackData;


class DataManager
{
public:
      
    DataManager() {}
      
    ~DataManager() {}
    
    void initialise(juce::File directory);
    
    void store(TrackData data);
    
    TrackData read(juce::String filename);
      
private:
    
    void execute(juce::String statement);
    
    void createTable();
    
    void printTrackData(TrackData data);

    bool initialised = false;
    void* database;
      
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DataManager)
};

#endif /* DataManager_hpp */
