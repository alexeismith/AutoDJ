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


#define DATABASE_FILENAME (".AutoDjData.db") ///< Filename for database, which is stored in the user's chosen music folder. The leading '.' hides the file on Mac.


/**
 SQLite3 database used for persistent storage of track data.
 */
class SqlDatabase
{
public:
      
    /** Constructor. */
    SqlDatabase() {}
      
    /** Destructor. */
    ~SqlDatabase();
    
    /** Initialises the database at the given directory. This creates a new database file if one doesn't already exist.
     
     @param[in] directory Folder in which to keep database
     
     @return False if unsuccessful */
    bool initialise(juce::File directory);
    
    /** Stores or updates track data in the database file.
     If an entry with the same filename already exists, it will be replaced with the provided data.
     
     @param[in] data Track information to store */
    void store(TrackInfo data);
    
    /** Searches the database for existing data on a given audio file.
     If returned hash is 0, the file is not present in the database.
     
     @return Existing track data from database */
    TrackInfo read(juce::String filename);
      
private:
    
    /** Executes the provided SQL statement and prints any errors that are returned.
     
     @param[in] statement SQL statement to execute */
    void execute(juce::String statement);
    
    /** Creates the Library table in the database file, for storing all track information.
     If the table already exists, nothing happens. */
    void createTable();
    
    /** Single apostrophes in SQL statement cause problems, so we convert
     them to double apostrophes before passing any strings to SQL.
     
     @param[in] text String to make SQL-safe
     
     @return String that can safely be used in an SQL statement */
    juce::String toSqlSafe(juce::String text) { return text.replace("'", "''", true); }
    
    /** Single apostrophes in SQL statement cause problems, so we convert
     them to double apostrophes before passing any strings to SQL.
     This function reverts this process of any strings that were stored in the database.
     
     @param[in] text String that was stored in the SQL database
     
     @return Original string */
    juce::String fromSqlSafe(juce::String text) { return text.replace("''", "'", true); }

    bool initialised = false; ///< Indiciates whether the database has been successfully initialised
    void* database; ///< Pointer to database data
    
      
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SqlDatabase) ///< JUCE macro to add a memory leak detector
};

#endif /* SqlDatabase_hpp */
