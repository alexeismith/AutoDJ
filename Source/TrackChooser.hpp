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


class TrackChooser
{
public:
    
    /** Constructor. */
    TrackChooser(DataManager* dm, RandomGenerator* random);
    
    /** Destructor. */
    ~TrackChooser() {}
    
    void initialise();
    
    TrackInfo* chooseTrack();
    
    void printChoice(TrackInfo* track);
    
private:
    
    void updatePosition();
    
    DataManager* dataManager = nullptr;
    TrackSorter* sorter = nullptr;
    
    int currentKey = -1;
    
    double currentBpm = -1.0;
    double currentGroove = -1.0;
    
    double velocityBpm = 0.0;
    double velocityGroove = 0.0;
    
    double accelerationBpm = 0.0;
    double accelerationGroove = 0.0;
    
    RandomGenerator* randomGenerator;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackChooser) ///< JUCE macro to add a memory leak detector
};


class KeySorter
{
public:
    
    KeySorter(int chromaKey) : reference(chromaKey) {}
    
    ~KeySorter() {}
    
    int compareElements(TrackInfo* first, TrackInfo* second);
    
private:
    
    CamelotKey reference;

};


#endif /* TrackChooser_hpp */
