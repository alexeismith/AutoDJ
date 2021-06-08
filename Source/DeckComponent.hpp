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


/**
 UI component representing a DJ deck, showing information on the loaded track, as well as its waveform.
 */
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
    
    /** Updates the state of the deck, including the waveform animation. */
    void update();
    
    /** Updates the track playhead position (current audio sample reached in track) */
    void logPlayheadPosition();
    
private:
    
    /** Loads a new track to be played.
     
     @param[in] track Pointer to the track's information */
    void load(Track* track);
    
    /** Unloads the current track. */
    void reset();
    
    /** Updates the transition markers shown in the track waveforms. */
    void updateMixMarkers();
    
    
    int deckId; ///< Unique ID of this deck
    
    std::atomic<bool> ready = false; ///< Thread-safe flag to indicate whether the track data is ready to display
    
    TrackProcessor* trackProcessor = nullptr; ///< Pointer to track audio processor associated with this deck
    
    Track track; ///< Information on the current track
    
    int playhead = 0; ///< Track playhead position (current audio sample reached in track)
    
    juce::String title; ///< Title of current track
    juce::String info; ///< String containing the track information to be displayed (other than artist/title)
    int titlePosY; ///< Y-coordinate at which to render the track title text
    int infoPosY; ///< Y-coordinate at which to render the track info text
    
    std::unique_ptr<WaveformView> waveform; ///< Component containing the large and thin audio waveforms
    
    juce::Colour colourBackground; ///< Base colour to paint as the background (this is transformed into a gradient - see paint())

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeckComponent) ///< JUCE macro to add a memory leak detector
};

#endif /* DeckComponent_hpp */
