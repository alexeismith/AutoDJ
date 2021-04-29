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

#include "ThirdParty/Quadtree/include/Quadtree.h"

#define Y_MULTIPLIER (0.25f)


struct GetBoxObj {
    quadtree::Box<float> operator()(TrackInfo* track) const
    {
        return quadtree::Box<float>(track->bpm, float(track->key)*Y_MULTIPLIER, 0.f, 0.f);
    }
};


using Quadtree = quadtree::Quadtree<TrackInfo*, GetBoxObj>;


class TrackSorter
{
public:
    
    TrackSorter();
    
    ~TrackSorter() {}
    
    void sort();
    
    void addAnalysed(TrackInfo* track);
    
    void remove(TrackInfo* track);
    
    // TODO: remove?
//    double getRangeBpm() { return abs(sortedBpm.getLast()->bpm - sortedBpm.getFirst()->bpm); }
//    double getRangeKey() { return abs(sortedKey.getLast()->key - sortedKey.getFirst()->key); }
    
    const juce::Array<TrackInfo*>& getSortedBpm() { return sortedBpm; }
    const juce::Array<TrackInfo*>& getSortedKey() { return sortedKey; }
    
    void testTree(double bpm, double key);
    
    TrackInfo* findClosest(float bpm, float key);
    
    int compareElements(TrackInfo* first, TrackInfo* second);
    
private:
    
    int compare(double first, double second);
    
    void addIntoSortedArray(juce::Array<TrackInfo*>& sortedArray, int category, TrackInfo* track);
    
    int findIndexOf(const juce::Array<TrackInfo*>& sortedArray, int category, double value);
    
    int findIndexOf(const juce::Array<TrackInfo*>& sortedArray, int category, double value, int& result);
    
    double getSortValue(TrackInfo* track);
    
    juce::CriticalSection lock;
    
    GetBoxObj getBoxObj;
    
    std::unique_ptr<Quadtree> tree;
    
    bool sorted = false;
    int sortCategory;
    
    juce::Array<TrackInfo*> sortedBpm;
    juce::Array<TrackInfo*> sortedKey;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackSorter)
};

#endif /* TrackSorter_hpp */
