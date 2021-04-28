//
//  TrackChooser.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 28/04/2021.
//

#ifndef TrackChooser_hpp
#define TrackChooser_hpp

#include <JuceHeader.h>

#include <random>

#include "TrackDataManager.hpp"


class TrackChooser
{
public:
    
    TrackChooser(TrackDataManager* dm);
    
    ~TrackChooser() {}
    
    TrackInfo chooseTrack() { return chooseTrackRandom(); }
    
    TrackInfo chooseTrackComplex();
    
    TrackInfo chooseTrackRandom();
    
private:
    
    void updateVelocity();
    
    TrackDataManager* dataManager = nullptr;
    TrackSorter* sorter = nullptr;
    
    double currentBpm = -1.0;
    double currentKey = -1.0;
    
    double velocityBpm = 0.0;
    double velocityKey = 0.0;
    
    double accelerationBpm = 0.0;
    double accelerationKey = 0.0;
    
    std::default_random_engine randomGenerator;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackChooser)
};


#endif /* TrackChooser_hpp */
