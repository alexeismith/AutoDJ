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
#include "AnalyserBeats.hpp"
#include "AnalyserKey.hpp"

class AnalysisManager
{
public:
    
    AnalysisManager(TrackDataManager* dm);
    
    ~AnalysisManager() {}
    
    void analyse(TrackData& track);
    
private:
    
    TrackDataManager* dataManager = nullptr;
    
    std::unique_ptr<AnalyserBeats> analyserBeats;
    std::unique_ptr<AnalyserKey> analyserKey;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalysisManager)
};

#endif /* AnalysisManager_hpp */
