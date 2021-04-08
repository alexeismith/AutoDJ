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
#include "ThirdParty/qm-dsp/dsp/tempotracking/TempoTrackV2.h"
#include "ThirdParty/qm-dsp/dsp/onsets/DetectionFunction.h"

class AnalysisManager
{
public:
    
    AnalysisManager(TrackDataManager* dm);
    
    ~AnalysisManager() {}
    
    void analyse(TrackData track);
    
private:
    
    TrackDataManager* dataManager = nullptr;
    
    TempoTrackV2 tempoTracker;
    std::unique_ptr<DetectionFunction> onsetAnalyser;
    DFConfig dfConfig;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalysisManager)
};

#endif /* AnalysisManager_hpp */
