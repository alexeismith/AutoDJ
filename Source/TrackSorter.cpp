//
//  TrackSorter.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 27/04/2021.
//

#include "TrackSorter.hpp"

// TODO: temp, get from elsewhere
#define BPM_MAX (300)
#define GROOVE_MAX (300)


TrackSorter::TrackSorter()
{
    tree.reset(new Quadtree(quadtree::Box<float>(0.f, 0.f, BPM_MAX, GROOVE_MAX)));
}


void TrackSorter::addAnalysed(TrackInfo* track)
{
    const juce::ScopedLock sl(lock);
    
    tree->add(track);
}


TrackInfo* TrackSorter::findClosestAndRemove(float bpm, float groove)
{
    TrackInfo* result = *tree->findClosest(quadtree::Box<float>(bpm, groove*GROOVE_MULTIPLIER, 0.f, 0.f));
    tree->remove(result);
    return result;
}
