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

#define WAVEFORM_HEIGHT (100)


class DeckComponent : public juce::Component, public juce::Button::Listener, public juce::HighResolutionTimer
{
public:
    
    DeckComponent(int id);
    
    ~DeckComponent() { stopTimer(); }
    
    void resized() override;
    
    void hiResTimerCallback() override;
    
    void buttonClicked(juce::Button* button) override;
    
    void loadTrack();
    
    void update(Track* track, int playhead, double timeStretch, double gain);
    
    void animate();
    
private:
    
    int deckId;
    
    Track* track;
    
    std::atomic<bool> newTrack = false;
    std::atomic<int> playhead = 0;
    std::atomic<double> timeStretch = 1.0;
    std::atomic<double> gain = 0.0;
    
    std::unique_ptr<WaveformComponent> waveform;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckComponent)
};

#endif /* DeckComponent_hpp */
