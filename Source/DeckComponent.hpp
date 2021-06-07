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

#include "WaveformView.hpp"

class DeckComponent : public juce::Component
{
public:
    
    DeckComponent(int id, TrackProcessor* processor);
    
    ~DeckComponent() {}
    
    void paint(juce::Graphics& g) override;
    
    void resized() override;
    
    void update();
    
    void logPlayheadPosition();
    
private:
    
    void load(Track* track);
    
    void reset();
    
    void setMixMarkers();
    
    int deckId;
    
    int playhead = 0;
    
    juce::String title;
    juce::String info;
    int titlePosY, infoPosY;
    
    std::atomic<bool> ready = false;
    
    TrackProcessor* trackProcessor = nullptr;
    
    std::unique_ptr<WaveformView> waveform;
    
    juce::Colour colourBackground;

    Track track;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeckComponent) ///< JUCE macro to add a memory leak detector
};

#endif /* DeckComponent_hpp */
