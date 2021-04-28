//
//  TrackSorter.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 27/04/2021.
//

#include "TrackSorter.hpp"


enum SortCategory : int
{
    bpm,
    key
};


void TrackSorter::sort()
{
    sortCategory = SortCategory::bpm;
    sortedBpm.sort(*this);
    
    sortCategory = SortCategory::key;
    sortedKey.sort(*this);
    
    sorted = true;
}


void TrackSorter::addAnalysed(TrackInfo* track)
{
    const juce::ScopedLock sl(lock);
    
    if (!sorted)
    {
        sortedBpm.add(track);
        sortedKey.add(track);
    }
    else
    {
        addIntoSortedArray(sortedBpm, SortCategory::bpm, track);
        addIntoSortedArray(sortedKey, SortCategory::key, track);
    }
}


int TrackSorter::compareElements(TrackInfo* first, TrackInfo* second)
{
    double firstValue = getSortValue(first);
    double secondValue = getSortValue(second);
    
    return compare(firstValue, secondValue);
}


int TrackSorter::compare(double first, double second)
{
    int result = first > second;
    if (first < second) return -1;
    return result;
}


void TrackSorter::addIntoSortedArray(juce::Array<TrackInfo*>& sortedArray, int category, TrackInfo* track)
{
    int result;
    sortCategory = category;
    
    int index = findIndexOf(sortedArray, category, getSortValue(track), result);
    
    if (result == 1)
        index += 1;
    sortedArray.insert(index, track);
}


// Adapted from https://gist.github.com/adamski/8d1c2719bcf0827b44a4
int TrackSorter::findIndexOf(const juce::Array<TrackInfo*>& sortedArray, int category, double value)
{
    if (sortedArray.size() == 0) return 0;
    
    sortCategory = category;
    
    // Conduct binary search
    int low, high, index;
    high = sortedArray.size();
    low = 0;
    double valueAtIndex;
    index = (high + low) / 2;

    juce::String debug;

    while (high != low+1)
    {
        valueAtIndex = getSortValue(sortedArray.getUnchecked(index));
        
        if (value == valueAtIndex)
        {
            return index;
        }
        else if (valueAtIndex < value)
        {
            low = index;
        }
        else
        {
            high = index;
        }
        index = (high + low) / 2;
    }

    // If there is an element above the current index, check whether it is closer to value
    if (sortedArray.size() > index+1)
    {
        double valueAboveIndex = getSortValue(sortedArray.getUnchecked(index+1));
        
        if (abs(valueAboveIndex - value) < abs(valueAtIndex - value))
            index += 1;
    }
    
    return index;
}


int TrackSorter::findIndexOf(const juce::Array<TrackInfo*>& sortedArray, int category, double value, int& result)
{
    if (sortedArray.size() == 0)
    {
        result = 0;
        return 0;
    }
    
    int index = findIndexOf(sortedArray, category, value);
    
    double valueAtIndex = getSortValue(sortedArray.getUnchecked(index));
    
    result = compare(value, valueAtIndex);
        
    return index;
}


void TrackSorter::remove(TrackInfo* const* track)
{
    sortedBpm.remove(track);
}


double TrackSorter::getSortValue(TrackInfo* track)
{
    switch (sortCategory)
    {
        case SortCategory::bpm:
            return track->bpm;
        case SortCategory::key:
            return track->key;
        default:
            jassert(false); // Unrecognized sortCategory
            return 0.0;
    };
}
