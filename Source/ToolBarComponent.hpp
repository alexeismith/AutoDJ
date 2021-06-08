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
    
    void timerCallback() override;
    
    void buttonClicked(juce::Button* button) override;
    
    void sliderValueChanged(juce::Slider* slider) override;
    
    void setCanPlay(bool state) { playPauseBtn->setEnabled(state); }
    
    void showSettings();
    
private:
    
    void setPlayPause(bool play);
    
    void changeView(ViewID view);
    
    juce::Component* mainComponent = nullptr;
    
    AudioProcessor* audioProcessor = nullptr;
    ArtificialDJ* dj = nullptr;

    std::unique_ptr<juce::Button> libraryBtn;
    std::unique_ptr<juce::Button> directionBtn;
    std::unique_ptr<juce::Button> mixBtn;
    
    ViewID currentView = ViewID::library, prevView = ViewID::library;
    
    std::unique_ptr<juce::ImageButton> playPauseBtn;
    std::unique_ptr<juce::ImageButton> skipBtn;
    std::unique_ptr<juce::ImageButton> settingsBtn;
    
    std::unique_ptr<juce::Slider> volumeSld;
    
    juce::Colour colourBackground;
    
    juce::Image playImg;
    juce::Image pauseImg;
    
    juce::Image volumeImg;
    juce::Rectangle<float> volumeArea;
    
    bool playing = false;
    bool ended = false;
    bool waitingForDJ = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToolBarComponent) ///< JUCE macro to add a memory leak detector
};

#endif /* ToolBarComponent_hpp */
