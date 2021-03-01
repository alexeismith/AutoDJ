//
//  DataManager.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 01/03/2021.
//

#include "DataManager.hpp"

extern "C" {
  #include <sqlite3.h>
}

void DataManager::initialise(juce::File directory)
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
    }
}


void DataManager::createTable()
{
    int errCode;
    char *zErrMsg = 0;
    
    juce::String statement("CREATE TABLE IF NOT EXISTS Library ("  \
                           "Filename TEXT UNIQUE NOT NULL," \
                           "Artist TEXT NOT NULL," \
                           "Title TEXT NOT NULL," \
                           "Length INT NOT NULL," \
                           "Bpm INT," \
                           "Key INT," \
                           "Energy INT)");

    errCode = sqlite3_exec((sqlite3*)database, statement.getCharPointer(), nullptr, 0, &zErrMsg);
    
    if( errCode != SQLITE_OK ){
       fprintf(stderr, "SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
    }
}
