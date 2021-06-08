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


/**
 Custom extension of JUCE progress bar, showing the audio analysis progress.
 Allows the user to pause/resume analysis.
 */
class AnalysisProgressBar : public juce::ProgressBar
{
public:
    
    /** Constructor. */
    AnalysisProgressBar(AnalysisManager* analysisManager);
    
    /** Destructor. */
    ~AnalysisProgressBar() {}
    
    /** Called by the JUCE message when this component is resized - set size/position of child components here. */
    void resized() override;
    
    /** Called by the JUCE message thread to paint this component.
     
     @param[in] g  JUCE graphics handler */
    void paint(juce::Graphics& g) override;
    
    /** Mouse input handler, called when the mouse enters the area of this component.
     
    @param[in] e Information about the input event that occured, such as mouse position */
    void mouseEnter(const juce::MouseEvent& e) override;
    
    /** Mouse input handler, called when the mouse exits the area of this component.
     
    @param[in] e Information about the input event that occured, such as mouse position */
    void mouseExit(const juce::MouseEvent& e) override;
    
    /** Mouse input handler, called when the mouse is pressed on this component.
     
    @param[in] e Information about the input event that occured, such as mouse position */
    void mouseDown(const juce::MouseEvent& e) override;
    
    /** Updates the analysis progress level.
     
     @param[in] progress Fraction indicating current analysis progress (0.0 to 1.0) */
    void update(double progress);
    
    /** Toggles play/pause state and notifies AnalysisManager. */
    void playPause();
    
    /** Forces a pause state and notifies AnalysisManager. */
    void pause();
    
private:
    
    /** Updates the text displayed in the progress bar, based on its current state. */
    void updateText();
    
    juce::CriticalSection lock; ///< RAII lock to ensure thread-safety while acessing data within this class
    
    AnalysisManager* analysisManager = nullptr; ///< Pointer to the audio analysis manager
    
    double progress = 0.0; ///< Fraction to track current analysis progress (0.0 to 1.0)
    
    bool paused = false; ///< Flag to track play/pause state of analysis
    
    bool mouseOver = false; ///< Flag to track whether the mouse is hovering over this component
    
    juce::Image playImg; ///< Play icon to indicate possible interaction when the mouse is over this component (while analysis is paused)
    juce::Image pauseImg; ///< Pause icon to indicate possible interaction when the mouse is over this component (while analysis is active)
    juce::Rectangle<float> imageArea; ///< Area in which to render the play/pause icon

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalysisProgressBar) ///< JUCE macro to add a memory leak detector
};

#endif /* AnalysisProgressBar_hpp */
