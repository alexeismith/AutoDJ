//
//  TransitionLine.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 12/05/2021.
//

#ifndef TransitionLine_hpp
#define TransitionLine_hpp

#include <JuceHeader.h>
#include "TrackDotComponent.hpp"


/**
 Animated line to show DJ transitions in the Direction view
 */
class TransitionLine : public juce::Component
{
public:
    
    /** Constructor. */
    TransitionLine() {}
    
    /** Destructor. */
    ~TransitionLine() {}
    
    /** Called by the JUCE message thread to paint this component.
     
     @param[in] g  JUCE graphics handler */
    void paint(juce::Graphics& g) override;
    
    /** Updates the line to show a new transition.
     
     @param[in] leader Pointer to dot representing the leading track in the transition
     @param[in] follower Pointer to dot representing the track being mixed in */
    void update(TrackDotComponent* leader, TrackDotComponent* follower);
    
    /** Triggers an update of the screen area which the line occupies (depends on the tracks being connected). */
    void updateBounds();
    
private:
    
    juce::Rectangle<float> bounds; ///< Screen area occupied by this line
    
    juce::Line<float> line; ///< Line vector to be drawn
    
    float dashLength[2] = { 5, 5 }; ///< Array to define the dashes in the line: a repeating pattern of 5 pixel dash, 5 pixel gap

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransitionLine) ///< JUCE macro to add a memory leak detector
};

#endif /* TransitionLine_hpp */
