//
//  DeckComponent.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/04/2021.
//

#ifndef DeckComponent_hpp
#define DeckComponent_hpp

#include <JuceHeader.h>

#include "WaveformComponent.hpp"
#include "TrackProcessor.hpp"

#define WAVEFORM_HEIGHT (100)


class DeckComponent : public juce::Component, public juce::Button::Listener
{
public:
    
    DeckComponent(TrackProcessor* processor, bool topDeck);
    
    ~DeckComponent() {}
    
    void resized() override;
    
    void buttonClicked(juce::Button* button) override;
    
    void update();
    
private:
    
    int currentTrackHash = 0;
    
    bool topDeck;
    
    TrackProcessor* trackProcessor;
    
    std::unique_ptr<WaveformComponent> waveform;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckComponent)
};

#endif /* DeckComponent_hpp */
