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
    
    /** Constructor. */
    AnalysisProgressBar(AnalysisManager* analysisManager);
    
    /** Destructor. */
    ~AnalysisProgressBar() {}
    
    /** Called by the JUCE message thread to paint this component.
     
     @param[in] g  JUCE graphics handler */
    void paint(juce::Graphics& g) override;
    
    void resized() override;
    
    void mouseEnter(const juce::MouseEvent &event) override;
    
    void mouseExit(const juce::MouseEvent &event) override;
    
    void mouseDown(const juce::MouseEvent &event) override;
    
    void update(double progress);
    
    void playPause();
    
    void pause();
    
private:
    
    void updateText();
    
    juce::CriticalSection lock;
    
    AnalysisManager* analysisManager = nullptr;
    
    double progress = 0.0;
    
    bool paused = false;
    
    bool mouseOver = false;
    
    juce::Rectangle<float> imageArea;
    
    juce::Image playImg;
    juce::Image pauseImg;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalysisProgressBar) ///< JUCE macro to add a memory leak detector
};

#endif /* AnalysisProgressBar_hpp */
