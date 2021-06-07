//
//  MixView.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/04/2021.
//

#ifndef MixView_hpp
#define MixView_hpp

#include <JuceHeader.h>

#include "DeckComponent.hpp"

class MixView : public juce::Component, public juce::Button::Listener, public juce::HighResolutionTimer
{
public:
    
    MixView(TrackProcessor** trackProcessors);
    
    ~MixView() { stopTimer(); }
    
    void hiResTimerCallback() override;
    
    void resized() override;
    
    void buttonClicked(juce::Button* button) override;
    
    void paintOverChildren(juce::Graphics &g) override;
    
private:
    
    juce::OwnedArray<DeckComponent> decks;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixView) ///< JUCE macro to add a memory leak detector
};

#endif /* MixView_hpp */
