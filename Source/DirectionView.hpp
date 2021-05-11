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


class DirectionView : public juce::Component
{
public:
    
    DirectionView(AnalysisManager* am);
    
    ~DirectionView() { reset(); }
    
    void paint(juce::Graphics& g) override;
    
    void resized() override;
    
    void updateData();
    
    void addAnalysed(TrackInfo* track);
    
    void calculatePositions();
    
    void reset();
    
private:
    
    juce::CriticalSection lock;
    
    std::unique_ptr<juce::TooltipWindow> toolTip;
    
    juce::OwnedArray<TrackDotComponent> dots;
    int numDotsAdded = 0;
    
    juce::Colour colourBackground;
    
    juce::Image logo;
    juce::Rectangle<float> logoArea;
    
    juce::Image axes;
    juce::Rectangle<float> axesArea;
    
    AnalysisManager* analysisManager;
    
    std::atomic<bool> needsUpdate = false;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirectionView)
};

#endif /* DirectionView_hpp */
