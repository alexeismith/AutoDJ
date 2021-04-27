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


typedef struct MixDirection
{
    double tempo = 0;
    double energy = 0;
    int currentKey = 0;
} MixDirection;

class TrackSorter
{
public:
    
    TrackSorter() {}
    
    ~TrackSorter() {}
    
    void sort();
    
    TrackInfo chooseTrack(MixDirection direction);
    
    void addAnalysed(TrackInfo* track);
    
private:
    
    void remove(TrackInfo* track);
    
    juce::CriticalSection lock;
    
    bool sorted = false;
    
    juce::Array<TrackInfo*> tracksAnalysed;
    juce::Array<TrackInfo*> sortedBpm;
    juce::Array<TrackInfo*> sortedKey;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackSorter)
};

#endif /* TrackSorter_hpp */
