//
//  TrackChooser.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 28/04/2021.
//

#ifndef TrackChooser_hpp
#define TrackChooser_hpp

#include <JuceHeader.h>

#include "TrackDataManager.hpp"


typedef struct MixDirection
{
    double tempoCurrent;
    double tempoChange;
    double energyCurrent;
    double energyChange;
    int keyCurrent = 0;
} MixDirection;


class TrackChooser
{
public:
    
    TrackChooser(TrackDataManager* dm);
    
    ~TrackChooser() {}
    
    TrackInfo chooseTrack(MixDirection direction);
    
private:

    TrackDataManager* dataManager = nullptr;
    TrackSorter* sorter = nullptr;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackChooser)
};


#endif /* TrackChooser_hpp */
