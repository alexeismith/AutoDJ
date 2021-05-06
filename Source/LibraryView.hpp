//
//  LibraryComponent.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/02/2021.
//

#ifndef LibraryComponent_hpp
#define LibraryComponent_hpp

#include "TrackDataManager.hpp"
#include "QueueTableComponent.hpp"
#include "AnalysisProgressBar.hpp"
#include "CommonDefs.hpp"

// TODO: temp
#include "WaveformScrollBar.hpp"

class LibraryView : public juce::Component, public juce::Timer
{
public:
    
    LibraryView(TrackDataManager* dataManager, double* loadingProgress);
    
    ~LibraryView() {}
    
    void resized() override;
    
    void timerCallback() override;
    
    void loadFiles();
    
    void analysisComplete() { analysisProgress->setVisible(false); }
    
private:
    
    void chooseFolder();
    
    std::unique_ptr<TrackTableComponent> trackTable;
    
    std::unique_ptr<AnalysisProgressBar> analysisProgress;
    
    TrackDataManager* dataManager;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LibraryView)
};

#endif /* LibraryComponent_hpp */
