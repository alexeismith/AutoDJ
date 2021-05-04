//
//  AnalysisProgressBar.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 05/05/2021.
//

#ifndef AnalysisProgressBar_hpp
#define AnalysisProgressBar_hpp

#include <JuceHeader.h>

#include "AnalysisManager.hpp"


class AnalysisProgressBar : public juce::ProgressBar
{
public:
    
    AnalysisProgressBar(AnalysisManager* analysisManager, double& progress);
    
    ~AnalysisProgressBar() {}
    
    void resized() override;
    
    void mouseEnter(const juce::MouseEvent &event) override;
    
    void mouseExit(const juce::MouseEvent &event) override;
    
    void mouseDown(const juce::MouseEvent &event) override;
    
private:
    
    AnalysisManager* analysisManager = nullptr;
    
    bool paused = false;
    
    std::unique_ptr<juce::Button> playPauseBtn; // TODO: just make this an image
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalysisProgressBar)
};

#endif /* AnalysisProgressBar_hpp */
