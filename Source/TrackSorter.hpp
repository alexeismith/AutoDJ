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


/**
 To mix tracks that are close in both tempo and groove, we need to search in 2D space to find tracks that are close to the current one.
 A user's music library could contain thousands of tracks, so we need to do this efficiently.
 A quadtree can represent 2D space very efficiently, using a tree of boxes. It recursively divides the space up into boxes until
 each point of interest - a track, in our case - is represented by a single box.
 
 The quadtree needs to know how to convert the points of interest into a 2D box representation - this is defined by GetBoxObj.
 We want the Quadtree to place tracks in 2D space based on their tempo and groove, so this GetBoxObj takes a TrackInfo pointer,
 and produces a quadtree::Box with a position equal to: (bpm, groove). The area is set to 0, since tracks have only one tempo/groove value.
 */
struct GetBoxObj {
    quadtree::Box<float> operator()(TrackInfo* track) const
    {
        return quadtree::Box<float>(track->bpm, float(track->groove)*GROOVE_MULTIPLIER, 0.f, 0.f);
    }
};


using Quadtree = quadtree::Quadtree<TrackInfo*, GetBoxObj>;


/**
 Wrapper class for a Quadtree, which sorts tracks in 2 dimensions: tempo and groove.
 Quadtree in use: https://github.com/MoonCollider/Quadtree
 
 To mix tracks that are close in both tempo and groove, we need to search in 2D space to find tracks that are close to the current one.
 A user's music library could contain thousands of tracks, so we need to do this efficiently.
 A quadtree can represent 2D space very efficiently, using a tree of boxes. It recursively divides the space up into boxes until
 each point of interest - a track, in our case - is represented by a single box.
 
 The functions for adding an element and searching for an element have O(Log N) complexity.
 https://www.geeksforgeeks.org/quad-tree/
 */
class TrackSorter
{
public:
    
    /** Constructor. */
    TrackSorter();
    
    /** Destructor. */
    ~TrackSorter() {}
    
    /** Adds a new track into the quadtree, with O(Log N) complexity.
     The track's tempo and groove determine its horizontal and vertical position in 2D space, respectively.
     
     @param[in] track Pointer to the track information to be added.
     */
    void addTrack(TrackInfo* track);
    
    /** Finds and removes the track closest to the specific 2D point (tempo, groove), with O(Log N) complexity.
     It is important to remove the track, so that we can search for other candidates to play near this point, without getting the same result every time.
     The candidates that are not played should be added back into the quadtree using addTrack().
     
     @param[in] bpm Horizontal component of the 2D position to search around
     @param[in] groove Vertical component of the 2D position to search around
     
     @return Pointer to the nearest track, which is now removed from the quadtree */
    TrackInfo* removeClosestTrack(float bpm, float groove);
    
    /** Clears all the quadtree data. */
    void reset();
    
private:
    
    juce::CriticalSection lock; ///< RAII lock to ensure thread-safety while acessing data within this class
    
    GetBoxObj getBoxObj; ///< Defines how to construct a 2D box from input data - see definition of GetBoxObj
    
    std::unique_ptr<Quadtree> tree; ///< The quadtree algorithm, from https://github.com/MoonCollider/Quadtree
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackSorter) ///< JUCE macro to add a memory leak detector
};

#endif /* TrackSorter_hpp */
