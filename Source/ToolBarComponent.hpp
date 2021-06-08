//
//  ToolBarComponent.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 29/05/2021.
//

#ifndef ToolBarComponent_hpp
#define ToolBarComponent_hpp

#include <JuceHeader.h>

#include "AudioProcessor.hpp"
#include "ArtificialDJ.hpp"
#include "CommonDefs.hpp"

#define TOOLBAR_HEIGHT (48) ///< The height in pixels of the tool bar


/**
 The tool bar that holds the primary navigation and audio controls in AutoDJ.
 Allows the user to switch between the different views.
 */
class ToolBarComponent : public juce::Component, public juce::Timer, public juce::Button::Listener, public juce::Slider::Listener
{
public:
    
    /** Constructor. */
    ToolBarComponent(juce::Component* mainComponent, AudioProcessor* audioProcessor, ArtificialDJ* dj);
    
    /** Destructor. */
    ~ToolBarComponent() {}
    
    /** Called by the JUCE message when this component is resized - set size/position of child components here. */
    void resized() override;

    /** Called by the JUCE message thread to paint this component.
     
     @param[in] g  JUCE graphics handler */
    void paint(juce::Graphics& g) override;
    
    /** JUCE timer callback, which checks the state of various flags and refreshes the toolbar accordingly. */
    void timerCallback() override;
    
    /** Input handler, called when a child button is pressed.
    
    @param[in] button Pointer to button that was pressed */
    void buttonClicked(juce::Button* button) override;
    
    /** Input handler, called when a child slider is moved.
    
    @param[in] slider Pointer to slider that was moved */
    void sliderValueChanged(juce::Slider* slider) override;
    
    /** Enables or disables the play/pause button.
     
     @param[in] state Determines new state of button */
    void setCanPlay(bool state) { playPauseBtn->setEnabled(state); }
    
    /** Shows the settings page. */
    void showSettings();
    
private:
    
    /** Sets the state of the play/pause button.
     
     @param[in] play Determines new state of button */
    void setPlayPause(bool play);
    
    /** Changes which view is displayed, adjusting the tab button colours accordingly.
     MainComponent is called to do the actual showing/hiding of views.
     
     @param[in] view ID of the view to be shown
     
     @see ViewID */
    void changeView(ViewID view);
    
    juce::Component* mainComponent = nullptr; ///< Pointer to the top-level app component
    AudioProcessor* audioProcessor = nullptr; ///< Pointer to the top-level audio processor
    ArtificialDJ* dj = nullptr; ///< Pointer to artificial DJ brain

    std::unique_ptr<juce::Button> libraryBtn; ///< Tab button to show the Library view
    std::unique_ptr<juce::Button> directionBtn; ///< Tab button to show the Direction view
    std::unique_ptr<juce::Button> mixBtn; ///< Tab button to show the Mix view
    std::unique_ptr<juce::ImageButton> settingsBtn; ///< Button to show settings page
    
    ViewID currentView = ViewID::library; ///< ID of the view currently showing
    ViewID prevView = ViewID::library; ///< ID of the view previously shown
    
    std::unique_ptr<juce::ImageButton> playPauseBtn; ///< Button to start/pause the DJ mix
    std::unique_ptr<juce::ImageButton> skipBtn; ///< Button to skip to the next mix event
    
    std::unique_ptr<juce::Slider> volumeSld; ///< Slider to control audio volume
    
    juce::Colour colourBackground; ///< Base colour to paint as the background
    
    juce::Image playImg; ///< Image icon to render as the play/pause button while paused
    juce::Image pauseImg; ///< Image icon to render as the play/pause button while playing
    
    juce::Image volumeImg; ///< Image icon to render next to the volume slider, to indicate its function
    juce::Rectangle<float> volumeArea; ///< Area in which to render volumeImg
    
    bool playing = false; ///< Flag to track the state of mix/audio playback
    bool ended = false; ///< Flag to track whether the mix has ended
    bool waitingForDJ = false; ///< Flag to track whether the DJ is generating a mix to begin playback (play/pause button is animated while this is true)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToolBarComponent) ///< JUCE macro to add a memory leak detector
};

#endif /* ToolBarComponent_hpp */
