//
//  SqlDatabase.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 03/03/2021.
//

#ifndef SqlDatabase_hpp
#define SqlDatabase_hpp

#include <JuceHeader.h>
#include "TrackData.hpp"

#define DATABASE_FILENAME ("AutoDjData.db")


class SqlDatabase
{
public:
      
    SqlDatabase() {}
      
    ~SqlDatabase();
    
    bool initialise(juce::File directory);
    
    void store(TrackData data);
    
    // If returned track data contains hash == 0, the filename is not present in the database
    TrackData read(juce::String filename);
      
private:
    
    void execute(juce::String statement);
    
    void createTable();

    bool initialised = false;
    void* database;
      
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SqlDatabase)
};

#endif /* SqlDatabase_hpp */
