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


/**
 The Mix view UI, showing information on the current tracks, and their waveforms.
 Most of the data shown is owned by the child DeckComponents.
 */
class MixView : public juce::Component, public juce::Button::Listener, public juce::HighResolutionTimer
{
public:
    
    /** Constructor. */
    MixView(TrackProcessor** trackProcessors);
    
    /** Destructor. */
    ~MixView() { stopTimer(); }
    
    /** Called by the JUCE message when this component is resized - set size/position of child components here. */
    void resized() override;
    
    /** Called by the JUCE message thread to paint the playhead line on top of the track waveforms.
    
    @param[in] g  JUCE graphics handler */
    void paintOverChildren(juce::Graphics &g) override;
    
    /** JUCE high-resolution timer callback, which is called on its own thread.
    Updates and animates the deck waveforms, without clogging the message thread. */
    void hiResTimerCallback() override;
    
    /** Input handler, called when a child button is pressed.
    
    @param[in] button Pointer to button that was pressed */
    void buttonClicked(juce::Button* button) override;
    
private:
    
    juce::OwnedArray<DeckComponent> decks; ///< Components representing each DJ deck / playing track
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixView) ///< JUCE macro to add a memory leak detector
};

#endif /* MixView_hpp */
