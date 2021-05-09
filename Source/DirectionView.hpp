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
    
    ~DirectionView() {}
    
    void paint(juce::Graphics& g) override;
    
    void resized() override;
    
    void updateData();
    
    void addAnalysed(TrackInfo* track);
    
    void calculatePositions();
    
private:
    
    std::unique_ptr<juce::TooltipWindow> toolTip;
    
    juce::OwnedArray<TrackDotComponent> dots;
    
    AnalysisManager* analysisManager;
    
    std::atomic<bool> needsUpdate = false;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirectionView)
};

#endif /* DirectionView_hpp */
