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


#define GROOVE_MULTIPLIER (20.f)


struct GetBoxObj {
    quadtree::Box<float> operator()(TrackInfo* track) const
    {
        return quadtree::Box<float>(track->bpm, float(track->groove)*GROOVE_MULTIPLIER, 0.f, 0.f);
    }
};


using Quadtree = quadtree::Quadtree<TrackInfo*, GetBoxObj>;


class TrackSorter
{
public:
    
    /** Constructor. */
    TrackSorter();
    
    /** Destructor. */
    ~TrackSorter() {}
    
    void addAnalysed(TrackInfo* track);
    
    TrackInfo* findClosestAndRemove(float bpm, float groove);
    
    void reset();
    
private:
    
    juce::CriticalSection lock; ///< RAII lock to ensure thread-safety while acessing data within this class
    
    GetBoxObj getBoxObj;
    
    std::unique_ptr<Quadtree> tree;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackSorter) ///< JUCE macro to add a memory leak detector
};

#endif /* TrackSorter_hpp */
