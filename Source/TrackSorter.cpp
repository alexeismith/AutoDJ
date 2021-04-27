//
//  TrackSorter.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 27/04/2021.
//

#include "TrackSorter.hpp"


void TrackSorter::sort()
{
    
    
    sorted = true;
}


TrackInfo TrackSorter::chooseTrack(MixDirection direction)
{
    const juce::ScopedLock sl(lock);
    
    return TrackInfo();
}


void TrackSorter::addAnalysed(TrackInfo* track)
{
    tracksAnalysed.add(track);
    
    if (!sorted) return;
    
}
