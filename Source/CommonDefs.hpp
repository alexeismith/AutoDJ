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

#define SUPPORTED_SAMPLERATE (44100)
#define TRACK_LENGTH_SECS_MIN (60) // 1 minute minimum length
#define TRACK_LENGTH_SECS_MAX (600) // 10 minutes maximum length
#define BEATS_PER_BAR (4)
#define NUM_TRACKS_MIN (4)

#define TOOLBAR_HEIGHT (40)


enum ComponentIDs : int
{
    libraryBtn,
    mixBtn,
    playPauseBtn,
    volumeSld,
    chooseFolderBtn
};


typedef struct AnalysisResults
{
    int minBpm = -1;
    int maxBpm = -1;
    float minGroove;
    float maxGroove;
} AnalysisResults;


namespace AutoDJ {

juce::String getLengthString(int secs);

juce::String getKeyName(int chromaKey);

juce::String getGrooveString(float groove);

int getBeatPeriod(int bpm);

// ============== Code taken from: https://www.geeksforgeeks.org/frequent-element-array/ ==============
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
