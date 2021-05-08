//
//  TrackChooser.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 28/04/2021.
//

#ifndef TrackChooser_hpp
#define TrackChooser_hpp

#include <JuceHeader.h>

#include "RandomGenerator.hpp"
#include "TrackDataManager.hpp"


class TrackChooser
{
public:
    
    TrackChooser(TrackDataManager* dm, RandomGenerator* random);
    
    ~TrackChooser() {}
    
    void initialise();
    
    TrackInfo* chooseTrack();
    
private:
    
    double getRandomGaussian(double stdDev = 1.0, double rangeLimit = -1.0, double shift = 0.0);
    
    void updatePosition();
    
    TrackDataManager* dataManager = nullptr;
    TrackSorter* sorter = nullptr;
    
    double currentBpm = -1.0;
    double currentGroove = -1.0;
    
    double velocityBpm = 0.0;
    double velocityGroove = 0.0;
    
    double accelerationBpm = 0.0;
    double accelerationGroove = 0.0;
    
    RandomGenerator* randomGenerator;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackChooser)
};


#endif /* TrackChooser_hpp */
