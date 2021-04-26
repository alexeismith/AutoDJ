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

class MixView : public juce::Component, public juce::Button::Listener, public juce::Timer
{
public:
    
    MixView(TrackProcessor** trackProcessors);
    
    ~MixView() {}
    
    void resized() override;
    
    void timerCallback() override;
    
    void buttonClicked(juce::Button* button) override;
    
//    void paint(juce::Graphics& g) override;
    
    void paintOverChildren(juce::Graphics &g) override;
    
private:
    
    juce::OwnedArray<DeckComponent> decks;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MixView)
};

#endif /* MixView_hpp */
