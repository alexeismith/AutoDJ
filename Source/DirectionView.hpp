//
//  DirectionView.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 09/05/2021.
//

#ifndef DirectionView_hpp
#define DirectionView_hpp

#include <JuceHeader.h>
#include "TrackDotComponent.hpp"
#include "AnalysisManager.hpp"
#include "TransitionLine.hpp"


/**
 The Direction view UI, which shows the music library distributed in terms of tempo and groove, with colour-coding against Camelot key signature.
 */
class DirectionView : public juce::Component, public juce::HighResolutionTimer
{
public:
    
    /** Constructor. */
    DirectionView(AnalysisManager* am);
    
    /** Destructor. */
    ~DirectionView() { reset(); }
    
    /** Called by the JUCE message when this component is resized - set size/position of child components here. */
    void resized() override;
    
    /** Called by the JUCE message thread to paint this component.
     
     @param[in] g  JUCE graphics handler */
    void paint(juce::Graphics& g) override;
    
    /** JUCE high-resolution timer callback, which is called on its own thread.
     Updates and animates the direction line, without clogging the message thread. */
    void hiResTimerCallback() override;
    
    /** Triggers a refresh of the track information. */
    void refresh();
    
    /** Adds a newly-analysed track to be displayed.
     
     @param[in] track Information of the track to be added */
    void addAnalysed(TrackInfo* track);
    
    /** Places every track in the 2D tempo/groove distribution. */
    void calculatePositions();
    
    /** Removes all tracks from the view. */
    void reset();
    
private:
    
    juce::CriticalSection lock; ///< RAII lock to ensure thread-safety while acessing data within this class
    
    AnalysisManager* analysisManager = nullptr; ///< Pointer to audio analysis manager
    
    TrackDotComponent* leader = nullptr; ///< Pointer to the dot which represents the leading track in the mix
    TrackDotComponent* follower = nullptr; ///< Pointer to the dot which represents the track being mixed in
    
    juce::OwnedArray<TrackDotComponent> dots; ///< Array of dots to display, representing tracks in the 2D tempo/groove distribution
    int numDotsAdded = 0; ///< Number of track dots added to the view
    
    TransitionLine transitionLine; ///< Animated line which connects tracks during a transition, shows the direction be moved
    
    std::unique_ptr<juce::TooltipWindow> toolTip; ///< JUCE tooltip component to show track names when the mouse is hovered over the dots
    
    juce::Colour colourBackground; ///< Base colour to paint as the background (this is transformed into a gradient - see paint())
    
    juce::Image logo; ///< AutoDJ logo image to render in the background
    juce::Rectangle<float> logoArea; ///< Area in which to render the AutoDJ logo
    
    juce::Image axes; ///< Image to delineate tempo and groove axes
    juce::Rectangle<float> axesArea; ///< Area in which to render the axes image
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DirectionView) ///< JUCE macro to add a memory leak detector
};

#endif /* DirectionView_hpp */
