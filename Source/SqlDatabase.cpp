//
//  SqlDatabase.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 03/03/2021.
//

#include "SqlDatabase.hpp"

extern "C" {
  #include <sqlite3.h>
}


bool SqlDatabase::initialise(juce::File directory)
{
    juce::File dbFile = juce::File(directory.getFullPathName() + "/" + DATABASE_FILENAME);
    
    if (!dbFile.existsAsFile())
        DBG("Creating new database file");
    
    // Open/create database
    sqlite3* db = 0;
    if (sqlite3_open((const char*) dbFile.getFullPathName().getCharPointer(), &db))
    {
        DBG("Error connecting to " + dbFile.getFullPathName());
    }
    else
    {
        database = db;
        createTable();
        initialised = true;
        return true;
    }
    
    return false;
}


void SqlDatabase::store(TrackData data)
{
    if (!initialised) jassert(false);
    
    std::stringstream ss;
    ss << "REPLACE INTO Library VALUES('" \
    << data.filename << "','" << data.artist << "','" << data.title << "'," << data.length << "," << data.analysed << "," << data.bpm << "," << data.key << "," << data.energy << ")";
    
    execute(ss.str());
    
    execute("SELECT COUNT(*) FROM Library");
}


TrackData SqlDatabase::read(juce::String filename)
{
    TrackData data;
    int errCode;
    char *zErrMsg = 0;
    sqlite3_stmt *statement;
    
    if (!initialised) jassert(false);
    
    std::stringstream ss;
    ss << "SELECT * FROM Library WHERE Filename = '" << filename << "'";
    
    execute(ss.str());
    
    errCode = sqlite3_prepare_v2((sqlite3*)database, ss.str().c_str(), -1, &statement, 0);
    if (errCode != SQLITE_OK)
    {
       fprintf(stderr, "SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
    }
    
    errCode = sqlite3_step(statement);
    if (errCode == SQLITE_ROW) {
        data.filename = juce::String(reinterpret_cast<const char*>(sqlite3_column_text(statement, 0)));
        data.artist = juce::String(reinterpret_cast<const char*>(sqlite3_column_text(statement, 1)));
        data.title = juce::String(reinterpret_cast<const char*>(sqlite3_column_text(statement, 2)));
        data.length = sqlite3_column_int(statement, 3);
        data.analysed = sqlite3_column_int(statement, 4);
        data.bpm = sqlite3_column_int(statement, 5);
        data.key = sqlite3_column_int(statement, 6);
        data.energy = sqlite3_column_int(statement, 7);
    }
    
    sqlite3_finalize(statement);
    
    return data;
}


void SqlDatabase::createTable()
{
    execute("CREATE TABLE IF NOT EXISTS Library ("  \
                           "Filename TEXT UNIQUE NOT NULL," \
                           "Artist TEXT NOT NULL," \
                           "Title TEXT NOT NULL," \
                           "Length INT NOT NULL," \
                           "Analysed INT NOT NULL," \
                           "Bpm INT," \
                           "Key INT," \
                           "Energy INT)");
}


void SqlDatabase::execute(juce::String statement)
{
    int errCode;
    char *zErrMsg = 0;
    
    errCode = sqlite3_exec((sqlite3*)database, statement.getCharPointer(), nullptr, 0, &zErrMsg);
    
    if (errCode != SQLITE_OK)
    {
       fprintf(stderr, "SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
    }
}
