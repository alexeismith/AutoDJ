//
//  AnalysisManager.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 26/03/2021.
//

#ifndef AnalysisManager_hpp
#define AnalysisManager_hpp

#include <JuceHeader.h>

#include "TrackDataManager.hpp"

class AnalysisManager
{
public:
    
    AnalysisManager(TrackDataManager* dm) : dataManager(dm) {}
    
    ~AnalysisManager() {}
    
    void analyse(TrackData track);
    
private:
    
    TrackDataManager* dataManager = nullptr;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalysisManager)
};

#endif /* AnalysisManager_hpp */
