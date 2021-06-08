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


class TrackDotComponent : public juce::Component, public juce::SettableTooltipClient, public juce::Timer
{
public:
    
    /** Constructor. */
    TrackDotComponent(TrackInfo* track);
    
    /** Destructor. */
    ~TrackDotComponent() {}
    
    TrackInfo* getTrack() { return info; }
    
    /** Called by the JUCE message thread to paint this component.
     
     @param[in] g  JUCE graphics handler */
    void paint(juce::Graphics& g) override;
    
    void timerCallback() override;
    
    void mouseEnter(const juce::MouseEvent &event) override;
    
    void mouseExit(const juce::MouseEvent &event) override;
    
    void update();
    
    juce::Point<float> getCentre();
    
    void setPosition(float xProportion, float yProportion);
    
    float getXProportion() { return xProportion; }
    float getYProportion() { return yProportion; }
    
    bool unplayed = true;
    
private:
    
    TrackInfo* info;
    
    int dotSize;
    
    juce::Colour colour;
    
    float xProportion, yProportion;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackDotComponent) ///< JUCE macro to add a memory leak detector
};

#endif /* TrackDotComponent_hpp */
