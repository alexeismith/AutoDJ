//
//  DirectionLine.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 12/05/2021.
//

#ifndef DirectionLine_hpp
#define DirectionLine_hpp

#include <JuceHeader.h>

#include "TrackDotComponent.hpp"

class DirectionLine : public juce::Component
{
public:
    
    DirectionLine() {}
    
    ~DirectionLine() {}
    
    void paint(juce::Graphics& g) override;
    
    void update(TrackDotComponent* leader, TrackDotComponent* follower);
    
    void updateBounds();
    
private:
    
    juce::Rectangle<float> bounds;
    
    juce::Line<float> line;
    
    float dashLength[2] = { 5, 5 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirectionLine)
};

#endif /* DirectionLine_hpp */
