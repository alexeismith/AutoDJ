//
//  CommonDefs.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 01/03/2021.
//

#ifndef CommonDefs_hpp
#define CommonDefs_hpp

#include <JuceHeader.h>

#include <unordered_map>

#define SUPPORTED_SAMPLERATE (44100) ///< Sample rate to use (Hz)
#define TRACK_LENGTH_SECS_MIN (60) ///< 1 minute minimum length
#define TRACK_LENGTH_SECS_MAX (600) ///< 10 minutes maximum length
#define BEATS_PER_BAR (4) ///< Number of beats per bar (4/4 time assumed)
#define NUM_TRACKS_MIN (6) ///< Minimum track required to launch mix


/** Enum to define a unique ID for each control (buttons & sliders). */
enum ComponentID : int
{
    libraryBtn,
    directionBtn,
    mixBtn,
    playPauseBtn,
    skipBtn,
    settingsBtn,
    volumeSld,
    chooseFolderBtn
};


/** Enum to define a unique ID for each view. */
enum ViewID : int
{
    library,
    direction,
    mix,
    settings
};


// Restrict the following functions to the AutoDJ:: namespace
namespace AutoDJ {

/** Generates a time length string from a given number of seconds, in the format: MM:SS.
 
 @param[in] secs Number of seconds, to convert to minutes and seconds
 
 @return Time length string in MM:SS */
juce::String getLengthString(int secs);

/** Generates a text representation of groove - 'Low', 'Med' or 'High' - which is determined
 by fixed ranges of groove that give a fairly balanced distribution across dance music.
 
 @param[in] groove Groove value of the track in question
 
 @return String indicating approximate level of groove */
juce::String getGrooveString(float groove);

/** Calculates the periodicity of beats at a given BPM, in samples (i.e., the number of samples per beat).
 
 @param[in] bpm Tempo to convert, in beats-per-minute
 
 @return Number of samples per beat */
int getBeatPeriod(int bpm);

// ============== Code taken from: https://www.geeksforgeeks.org/frequent-element-array/ ==============
/** Finds the most common value in an array of any data type.
 
 @param[in] data Pointer to array data
 @param[in] size Number of elements in the array
 
 @return Value of most common element in the array */
template <typename Type>
Type mostCommonValue(Type* data, int size)
{
    // Insert all elements in hash.
    std::unordered_map<Type, int> hash;
    for (int i = 0; i < size; i++)
        hash[data[i]]++;

    // find the max frequency
    int max_count = 0;
    Type res;
    for (auto i : hash) {
        if (max_count < i.second) {
            res = i.first;
            max_count = i.second;
        }
    }

    return res;
}
// ====================================================================================================

} /* namespace AutoDJ */

#endif /* CommonDefs_hpp */
