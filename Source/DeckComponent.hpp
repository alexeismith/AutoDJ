//
//  DeckComponent.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/04/2021.
//

#ifndef DeckComponent_hpp
#define DeckComponent_hpp

#include <JuceHeader.h>

#include "TrackProcessor.hpp"

#include "WaveformComponent.hpp"

#define WAVEFORM_HEIGHT (100)


class DeckComponent : public juce::Component, public juce::Button::Listener
{
public:
    
    DeckComponent(int id, TrackProcessor* processor);
    
    ~DeckComponent() {}
    
    void resized() override;
    
    void buttonClicked(juce::Button* button) override;
    
    void load(Track* track);
    
    void update();
    
    void flipWaveform() { waveform->flipImage(); }
    
private:
    
    int deckId;
    
    TrackProcessor* trackProcessor = nullptr;
    
    std::unique_ptr<WaveformComponent> waveform;
    std::unique_ptr<WaveformLoadThread> waveformLoader;

    Track* track;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckComponent)
};

#endif /* DeckComponent_hpp */
