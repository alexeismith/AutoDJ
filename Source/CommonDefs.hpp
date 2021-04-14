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
#define TRACK_LENGTH_SECS_MIN (60)
#define TRACK_LENGTH_SECS_MAX (600)
#define BEATS_PER_BAR (4)

enum ComponentIDs : int
{
    chooseFolder
};

namespace AutoDJ {

juce::String getLengthString(int secs);

juce::String getKeyName(int chromaKey);


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
