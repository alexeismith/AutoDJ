//
//  SqlDatabase.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 03/03/2021.
//

#ifndef SqlDatabase_hpp
#define SqlDatabase_hpp

#include <JuceHeader.h>
#include "TrackInfo.hpp"


#define DATABASE_FILENAME (".AutoDjData.db")


class SqlDatabase
{
public:
      
    /** Constructor. */
    SqlDatabase() {}
      
    /** Destructor. */
    ~SqlDatabase();
    
    bool initialise(juce::File directory);
    
    void store(TrackInfo data);
    
    // If returned track data contains hash == 0, the filename is not present in the database
    TrackInfo read(juce::String filename);
      
private:
    
    void execute(juce::String statement);
    
    void createTable();
    
    juce::String toSqlSafe(juce::String text) { return text.replace("'", "''", true); }
    
    juce::String fromSqlSafe(juce::String text) { return text.replace("''", "'", true); }

    bool initialised = false;
    void* database;
      
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SqlDatabase) ///< JUCE macro to add a memory leak detector
};

#endif /* SqlDatabase_hpp */
