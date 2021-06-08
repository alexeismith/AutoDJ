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
    
    /** Constructor. */
    DeckComponent(int id, TrackProcessor* processor);
    
    /** Destructor. */
    ~DeckComponent() {}
    
    /** Called by the JUCE message when this component is resized - set size/position of child components here. */
    void resized() override;
    
    /** Called by the JUCE message thread to paint this component.
     
     @param[in] g  JUCE graphics handler */
    void paint(juce::Graphics& g) override;
    
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
    
    TrackProcessor* trackProcessor = nullptr; ///< Pointer to 
    
    std::unique_ptr<WaveformView> waveform;
    
    juce::Colour colourBackground; ///< Base colour to paint as the background (this is transformed into a gradient - see paint())

    Track track;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeckComponent) ///< JUCE macro to add a memory leak detector
};

#endif /* DeckComponent_hpp */
