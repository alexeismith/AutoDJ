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
    
    TrackDotComponent(TrackInfo* track);
    
    ~TrackDotComponent() {}
    
    TrackInfo* getTrack() { return info; }
    
    void paint(juce::Graphics& g) override;
    
    void timerCallback() override;
    
    void mouseEnter(const juce::MouseEvent &event) override;
    
    void mouseExit(const juce::MouseEvent &event) override;
    
    void update();
    
    void setPosition(float xProportion, float yProportion);
    
    float getXProportion() { return xProportion; }
    float getYProportion() { return yProportion; }
    
private:
    
    TrackInfo* info;
    
    int dotSize;
    
    juce::Colour colour;
    
    float xProportion, yProportion;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackDotComponent)
};

#endif /* TrackDotComponent_hpp */
