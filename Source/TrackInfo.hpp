//
//  TrackInfo.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 03/03/2021.
//

#ifndef TrackInfo_h
#define TrackInfo_h

#include <JuceHeader.h>


/**
 Data structure used to represent tracks/songs throughout AutoDJ
 */
class TrackInfo
{
public:
    
    /** Constructor. */
    TrackInfo() {}
    
    /** Destructor. */
    ~TrackInfo() {}
    
    /** Calculates the audio sample that coincides with the provided beat number.
     Assumes first beat in track is beat 0.
     Track must be analysed before using this function.
     
     @param[in] beat Number of the musical beat to find
     
     @return Audio sample number at beat */
    int getSampleOfBeat(int beat);
    
    /** Calculates the audio sample that coincides with the first downbeat in the track.
    Track must be analysed before using this function.
    
    @return Audio sample number at first downbeat */
    int getFirstDownbeat() { return getSampleOfBeat(downbeat); }
    
    /** Calculates the index of the downbeat nearest to the given audio sample,
     where the first downbeat would be number 0.
     Track must be analysed before using this function.
     
     @param[in] sample Audio sample number to check against
     
     @return Index of nearest downbeat to sample */
    int getNearestDownbeat(int sample);
    
    /** Finds the number of audio samples per beat, which is
     determined by the track's BPM and the global sample rate.
     Track must be analysed before using this function.
     
     @return Number of audio samples per beat */
    int getBeatPeriod();
    
    /** Gets the number of audio samples per musical bar, which is
     determined by the number of audio samples per beat.
     Track must be analysed before using this function.
     
     @return Number of audio samples per bar */
    int getBarLength();
    
    /** Gets the length of the track in audio samples.
     
     @return Number of audio samples in track */
    int getLengthSamples();
    
    /** Generates the track's filename as a JUCE string.
    The filename is stored as a char array, so that it is given an explicit memory location that doesn't move or resize
    - this allows TrackInfo pointers to reference the same object in the TrackInfo array throughout runtime (see DataManager::tracks)
    
    @return Filename of track */
    juce::String getFilename() { return juce::String(juce::CharPointer_UTF8(filename)); }
    
    /** Generates the track's artist metadata as a JUCE string.
    The metadata is stored as a char array, so that it is given an explicit memory location that doesn't move or resize
    - this allows TrackInfo pointers to reference the same object in the TrackInfo array throughout runtime (see DataManager::tracks)
    
    @return Artist name */
    juce::String getArtist() { return juce::String(juce::CharPointer_UTF8(artist)); }
    
    /** Generates the track's title metadata as a JUCE string.
    The metadata is stored as a char array, so that it is given an explicit memory location that doesn't move or resize
    - this allows TrackInfo pointers to reference the same object in the TrackInfo array throughout runtime (see DataManager::tracks)
    
    @return Title */
    juce::String getTitle();
    
    /** Generates the track's artist and title metadata as a JUCE string.
    The metadata is stored as a char array, so that it is given an explicit memory location that doesn't move or resize
    - this allows TrackInfo pointers to reference the same object in the TrackInfo array throughout runtime (see DataManager::tracks)
    
    @return Artist and title */
    juce::String getArtistTitle();
    
    /** Sets the track's filename from a JUCE string.
    The filename is stored as a char array, so that it is given an explicit memory location that doesn't move or resize
    - this allows TrackInfo pointers to reference the same object in the TrackInfo array throughout runtime (see DataManager::tracks)
    
    @param[in] text Filename to store */
    void setFilename(juce::String text) { strcpy(filename, text.getCharPointer()); }
    
    /** Sets the track's artist from a JUCE string.
    The artist is stored as a char array, so that it is given an explicit memory location that doesn't move or resize
    - this allows TrackInfo pointers to reference the same object in the TrackInfo array throughout runtime (see DataManager::tracks)
    
    @param[in] text Artist to store */
    void setArtist(juce::String text) { strcpy(artist, text.getCharPointer()); }
    
    /** Sets the track's title from a JUCE string.
    The title is stored as a char array, so that it is given an explicit memory location that doesn't move or resize
    - this allows TrackInfo pointers to reference the same object in the TrackInfo array throughout runtime (see DataManager::tracks)
    
    @param[in] text Title to store */
    void setTitle(juce::String text) { strcpy(title, text.getCharPointer()); }
    
    
    int hash = 0; ///< Unique hash of the track's file audio file, computed using XXHash32 algorithm

    int length = 0; ///< Total length in seconds
    bool analysed = false; ///< Indicates whether analysis has been performed
    bool played = false; ///< Indicates whether the track has been played during the current DJ performance
    bool playing = false; ///< Indicates whether the track is currently playing
    int bpm = -1; ///< Tempo in beats-per-minute
    int beatPhase = -1; ///< Phase of beat grid, measured in audio samples, as an offset from the very first sample
    int downbeat = -1; ///< Index of first downbeat, indicating which of the first four beats is a downbeat (can also be thought of as the phase of downbeats)
    int key = -1; ///< Musical key signature, chromatic key is used for storage, rather than Camelot, because it is a simpler representation
    float groove = -1.f; ///< The track's danceability metric
    
    
private:
    
    /** The track metadata is stored as char arrays, which have explicit memory locations that doesn't move or resize
    - this allows TrackInfo pointers to reference the same object in the TrackInfo array throughout runtime (see DataManager::tracks)*/
    
    char filename[FILENAME_MAX]; ///< Filename of track
    char artist[FILENAME_MAX]; ///< Artist who created track
    char title[FILENAME_MAX]; ///< Title of track
};

#endif /* TrackInfo_h */
