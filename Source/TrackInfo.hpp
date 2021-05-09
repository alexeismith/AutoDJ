//
//  TrackInfo.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 03/03/2021.
//

#ifndef TrackInfo_h
#define TrackInfo_h

#include <JuceHeader.h>


class TrackInfo
{
public:
    
    TrackInfo() {}
    
    ~TrackInfo() {}
    
    int getSampleOfBeat(int beat);
    
    int getFirstDownbeat() { return getSampleOfBeat(downbeat); }
    
    int getNearestDownbeat(int sample);
    
    int getBeatPeriod();
    
    int getBarLength();
    
    int getLengthSamples(); // TODO: make sure this is used where possible
    
    juce::String getFilename() { return juce::String(juce::CharPointer_UTF8(filename)); }
    juce::String getArtist() { return juce::String(juce::CharPointer_UTF8(artist)); }
    juce::String getTitle();
    juce::String getArtistTitle();
    
    void setFilename(juce::String text) { strcpy(filename, text.getCharPointer()); }
    void setArtist(juce::String text) { strcpy(artist, text.getCharPointer()); }
    void setTitle(juce::String text) { strcpy(title, text.getCharPointer()); }
    
//    int getLastDownbeat(); // TODO: if useful, make sure this works
    
    
    int hash = 0; // Hash of the raw data, computed using XXHash32 algorithm

    int length = 0; // Total length in seconds
    bool analysed = false; // Indicates whether analysis has been performed
    bool played = false; // Indicates whether the track has been played this session
    bool playing = false; // Indicates whether the track is currently playing
    int bpm = -1; // Tempo in beats per minute
    int beatPhase = -1;
    int downbeat = -1;
    int key = -1; // Musical key signature TODO: how to represent camelot?
    float groove = -1.f; // The track's 'danceability'
    
    
private:
    
    char filename[FILENAME_MAX];
    char artist[FILENAME_MAX];
    char title[FILENAME_MAX];
};

#endif /* TrackInfo_h */
