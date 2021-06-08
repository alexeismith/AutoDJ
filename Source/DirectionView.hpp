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
#include "DirectionLine.hpp"


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
    
    void updateData();
    
    void addAnalysed(TrackInfo* track);
    
    void calculatePositions();
    
    void reset();
    
private:
    
    juce::CriticalSection lock;
    
    std::unique_ptr<juce::TooltipWindow> toolTip;
    
    juce::OwnedArray<TrackDotComponent> dots;
    int numDotsAdded = 0;
    
    juce::Colour colourBackground; ///< Base colour to paint as the background (this is transformed into a gradient - see paint())
    
    juce::Image logo;
    juce::Rectangle<float> logoArea;
    
    juce::Image axes;
    juce::Rectangle<float> axesArea;
    
    AnalysisManager* analysisManager;
    
    std::atomic<bool> needsUpdate = false;
    
    TrackDotComponent* leader = nullptr; ///< Pointer to 
    TrackDotComponent* follower = nullptr; ///< Pointer to 
    
    DirectionLine directionLine;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DirectionView) ///< JUCE macro to add a memory leak detector
};

#endif /* DirectionView_hpp */
