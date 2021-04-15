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


SqlDatabase::~SqlDatabase()
{
    if (initialised)
        sqlite3_close((sqlite3*)database);
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
    << toSqlSafe(data.filename) << "','" << data.hash << "','" << toSqlSafe(data.artist) << "','" << toSqlSafe(data.title) << "'," << data.length << "," << data.analysed << "," << data.bpm << "," << data.beatPhase << "," << data.downbeat << "," << data.key << "," << data.energy << ")";
    
    execute(ss.str());
}


TrackData SqlDatabase::read(juce::String filename)
{
    TrackData data;
    int errCode;
    char *zErrMsg = 0;
    sqlite3_stmt *statement;
    
    if (!initialised) jassert(false);
    
    std::stringstream ss;
    ss << "SELECT * FROM Library WHERE Filename = '" << toSqlSafe(filename) << "'";
    
    execute(ss.str());
    
    errCode = sqlite3_prepare_v2((sqlite3*)database, ss.str().c_str(), -1, &statement, 0);
    if (errCode != SQLITE_OK)
    {
       fprintf(stderr, "SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
    }
    
    errCode = sqlite3_step(statement);
    if (errCode == SQLITE_ROW)
    {
        data.filename = fromSqlSafe(juce::CharPointer_UTF8(reinterpret_cast<const char*>(sqlite3_column_text(statement, 0))));
        data.hash = sqlite3_column_int(statement, 1);
        data.artist = fromSqlSafe(juce::CharPointer_UTF8(reinterpret_cast<const char*>(sqlite3_column_text(statement, 2))));
        data.title = fromSqlSafe(juce::CharPointer_UTF8(reinterpret_cast<const char*>(sqlite3_column_text(statement, 3))));
        data.length = sqlite3_column_int(statement, 4);
        data.analysed = sqlite3_column_int(statement, 5);
        data.bpm = sqlite3_column_int(statement, 6);
        data.beatPhase = sqlite3_column_int(statement, 7);
        data.downbeat = sqlite3_column_int(statement, 8);
        data.key = sqlite3_column_int(statement, 9);
        data.energy = sqlite3_column_int(statement, 10);
    }
    
    sqlite3_finalize(statement);
    
    return data;
}


void SqlDatabase::createTable()
{
    execute("CREATE TABLE IF NOT EXISTS Library ("  \
                           "filename TEXT UNIQUE NOT NULL," \
                           "hash INT NOT NULL," \
                           "artist TEXT NOT NULL," \
                           "title TEXT NOT NULL," \
                           "length INT NOT NULL," \
                           "analysed INT NOT NULL," \
                           "bpm INT," \
                           "beatPhase INT," \
                           "downbeat INT," \
                           "key INT," \
                           "energy INT)");
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
