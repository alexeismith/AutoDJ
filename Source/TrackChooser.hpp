//
//  TrackChooser.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 28/04/2021.
//

#ifndef TrackChooser_hpp
#define TrackChooser_hpp

#include <JuceHeader.h>
#include "CamelotKey.hpp"
#include "RandomGenerator.hpp"
#include "DataManager.hpp"


/**
 Chooses tracks to play in sequence.
 Takes into account tempo, groove, key signature and randomness.
 */
class TrackChooser
{
public:
    
    /** Constructor. */
    TrackChooser(DataManager* dm, RandomGenerator* random);
    
    /** Destructor. */
    ~TrackChooser() {}
    
    /** Chooses an initial tempo, groove and key, ready for the first choice of track. */
    void initialise();
    
    /** Chooses the next track, taking into account tempo, groove, key signature and randomness.
     
     @return Pointer to the information of the chosen track */
    TrackInfo* chooseTrack();
    
    /** Prints the supplied track information to the debug console.
     
     @param[in] track Pointer to information to print */
    void printChoice(TrackInfo* track);
    
private:
    
    /** Shifts the tempo and groove position, based on the current velocity. */
    void updatePosition();
    
    DataManager* dataManager = nullptr; ///< Pointer to the app's track data manager
    TrackSorter* sorter = nullptr; ///< Pointer to a quadtree which sorts tracks based on tempo and groove
    RandomGenerator* randomGenerator; ///< Pointer to random number engine
    
    int currentKey = -1; ///< Key signature of the previous track choice
    
    double currentBpm = -1.0; ///< Tempo of the previous track choice, in beats-per-minute
    double currentGroove = -1.0; ///< Groove value of the previous track choice
    
    double velocityBpm = 0.0; ///< Currenty BPM velocity, which is added to currentBpm in between each track choice
    double velocityGroove = 0.0; ///< Currenty groove velocity, which is added to currentGroove in between each track choice
    
    double accelerationBpm = 0.0; ///< Currenty BPM acceleration, which is adjusted in between each track choice
    double accelerationGroove = 0.0; ///< Currenty groove acceleration, which is adjusted in between each track choice
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackChooser) ///< JUCE macro to add a memory leak detector
};


/**
 Simple class for sorting tracks, in terms of their key's compatibility with a reference key.
 Conforms to JUCE's element comparator template:
 https://docs.juce.com/master/tutorial_table_list_box.html#tutorial_table_list_box_sorting_data
 */
class KeySorter
{
public:
    
    /** Constructor. */
    KeySorter(CamelotKey referenceKey) : reference(referenceKey) {}
    
    /** Destructor. */
    ~KeySorter() {}
    
    /** Element comparator function.
    
    @param[in] first Pointer to first track to be compared
    @param[in] second Pointer to second track to be compared
    
    @return -1 if first element sorts higher, 1 if second element sorts higher, 0 if it's a draw */
    int compareElements(TrackInfo* first, TrackInfo* second);
    
private:
    
    CamelotKey reference; ///< Reference key to sort elements against

};


#endif /* TrackChooser_hpp */
