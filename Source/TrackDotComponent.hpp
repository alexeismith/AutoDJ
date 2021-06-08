//
//  TrackDotComponent.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 09/05/2021.
//

#ifndef TrackDotComponent_hpp
#define TrackDotComponent_hpp

#include "TrackInfo.hpp"
#include "CamelotKey.hpp"


/**
 Circle element to represent tracks in the Direction view. Animated while the track is playing, and colour-coded to its Camelot key signature.
 */
class TrackDotComponent : public juce::Component, public juce::SettableTooltipClient, public juce::Timer
{
public:
    
    /** Constructor. */
    TrackDotComponent(TrackInfo* track);
    
    /** Destructor. */
    ~TrackDotComponent() {}
    
    /** Called by the JUCE message thread to paint this component.
     
     @param[in] g  JUCE graphics handler */
    void paint(juce::Graphics& g) override;
    
    /** JUCE timer callback, which animates the dot when its associated track is playing. */
    void timerCallback() override;
    
    /** Mouse input handler, called when the mouse enters the area of this component.
     
    @param[in] e Information about the input event that occured, such as mouse position */
    void mouseEnter(const juce::MouseEvent& e) override;
    
    /** Mouse input handler, called when the mouse exits the area of this component.
     
    @param[in] e Information about the input event that occured, such as mouse position */
    void mouseExit(const juce::MouseEvent& e) override;
    
    /** Fetches the information of the track associated with this dot.
     
     @return Pointer to track information */
    TrackInfo* getTrack() { return info; }
    
    /** Triggers an update of the dot's state, to check whether the track is playing, or played. */
    void update();
    
    /** Fetches the coordinate of the dot's centre point.
     
     @return Screen coordinates of centre */
    juce::Point<float> getCentre();
    
    /** Sets a new position for the centre of the dot, in terms of its parent's area.
     Proportion is used so that the parent can be resized without recalculating this position.
     
     @param[in] xProportion Position across the parent's width that this dot will be placed at
     @param[in] yProportion Position down the parent's height that this dot will be placed at */
    void setPosition(float xProportion, float yProportion);
    
    /** Fetches the horizontal position of this dot, in terms of its parent's width.
     
     @return Position across the parent's width that this dot should be placed at */
    float getXProportion() { return xProportion; }
    
    /** Fetches the vertical position of this dot, in terms of its parent's height.
    
    @return Position across the parent's height that this dot should be placed at */
    float getYProportion() { return yProportion; }
    
private:
    
    TrackInfo* info; ///< Pointer to information on the associated track
    
    int diameter; ///< Current size of the dot - changes based on animation and mouse position
    
    juce::Colour colour; ///< Colour of the dot, determined by the track's Camelot key signature
    
    float xProportion; ///< Position across the parent's width that this dot should be placed at
    float yProportion; ///< Position down the parent's height that this dot should be placed at
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackDotComponent) ///< JUCE macro to add a memory leak detector
};

#endif /* TrackDotComponent_hpp */
