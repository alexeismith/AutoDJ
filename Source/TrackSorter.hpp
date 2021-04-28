//
//  TrackSorter.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 27/04/2021.
//

#ifndef TrackSorter_hpp
#define TrackSorter_hpp

#include <JuceHeader.h>

#include "TrackInfo.hpp"


class TrackSorter
{
public:
    
    TrackSorter() {}
    
    ~TrackSorter() {}
    
    void sort();
    
    void addAnalysed(TrackInfo* track);
    
    const juce::Array<TrackInfo*>& getSortedBpm() { return sortedBpm; }
    const juce::Array<TrackInfo*>& getSortedKey() { return sortedKey; }
    
    int compareElements(TrackInfo* first, TrackInfo* second);
    
private:
    
    int compare(double first, double second);
    
    void addIntoSortedArray(juce::Array<TrackInfo*>& sortedArray, int category, TrackInfo* track);
    
    int findIndexOf(const juce::Array<TrackInfo*>& sortedArray, int category, double value);
    
    int findIndexOf(const juce::Array<TrackInfo*>& sortedArray, int category, double value, int& result);
    
    void remove(TrackInfo* const* track);
    
    double getSortValue(TrackInfo* track);
    
    juce::CriticalSection lock;
    
    bool sorted = false;
    int sortCategory;
    
    juce::Array<TrackInfo*> sortedBpm;
    juce::Array<TrackInfo*> sortedKey;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackSorter)
};

#endif /* TrackSorter_hpp */
