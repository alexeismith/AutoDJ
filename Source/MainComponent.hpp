#pragma once

#include <JuceHeader.h>

#include "AudioProcessor.hpp"
#include "LibraryView.hpp"
#include "DirectionView.hpp"
#include "MixView.hpp"
#include "GraphComponent.hpp"
#include "ArtificialDJ.hpp"
#include "AnalysisProgressBar.hpp"
#include "ToolBarComponent.hpp"

// Set the following macro to show the debugging graph (see GraphComponent.hpp)
//#define SHOW_GRAPH

/**
 The top-level JUCE window, which owns all other objects in the application.
 Handles switching between views, audio settings, and selecting the user's music folder at launch.
 */
class MainComponent : public juce::AudioAppComponent, public juce::Timer, public juce::Button::Listener
{
public:
    
    /** Constructor - instantiates most of the app's top-level objects. */
    MainComponent();
    
    /** Destructor - called when the app wants to exit. */
    ~MainComponent() override;
    
    /** Called by the JUCE message when this component is resized - set size/position of child components here. */
    void resized() override;
    
    /** Called by the JUCE message thread to paint this component.
     
     @param[in] g  JUCE graphics handler */
    void paint(juce::Graphics& g) override;

    /** Usually called when audio settings change, to prepare the audio processing pipeline.
     
     @param[in] samplesPerBlockExpected Expected block size for audio processing
     @param[in] sampleRate Sample rate for audio processing */
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    
    /** Top-level audio processing loop - calls AudioProcessor::getNextAudioBlock().
     
     @param[in] bufferToFill Buffer to fill with desired audio output samples
     
     @see AudioProcessor::getNextAudioBlock */
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    
    /** Called when the audio device stops or restarts (Currently unused). */
    void releaseResources() override {}

    /** JUCE timer callback, used to check the state of various flags and refresh the app state accordingly. */
    void timerCallback() override;
    
    /** Input handler, called when a child button is pressed.
     
     @param[in] button Pointer to button that was pressed */
    void buttonClicked(juce::Button* button) override;
    
    /** Checks whether the current audio settings are valid - currently only checks sample rate.
     
     @param[in] showError Indicates whether an error message should be displayed if the settings are invalid
     
     @return Validity of current audio settings */
    bool validAudioSettings(bool showError = false);
    
    /** Changes which view is shown to the user, usually called by ToolBarComponent.
     
     @param[in] view ID of the view to be shown
     
     @see ViewID, ToolBarComponent::changeView */
    void changeView(ViewID view);

private:
    
    /** Resets the state of the entire DJ mix, ready for a new performance. */
    void resetMix();
    
    /** Sets the colour scheme of the app. */
    void setAppearance();
    
    /** Triggers the process of choosing a music folder. */
    void chooseFolder();
    
    
    juce::AudioDeviceManager audioSettings; ///< Stores the audio hardware settings
    std::unique_ptr<juce::AudioDeviceSelectorComponent> audioSettingsSelector; ///< Component for changing the audio hardware settings
    
    juce::ComponentBoundsConstrainer sizeLimits; ///< Stores the window size limits
    
    std::unique_ptr<LibraryView> libraryView; ///< Library view UI
    std::unique_ptr<DirectionView> directionView; ///< Direction view UI
    std::unique_ptr<MixView> mixView; ///< Mix view UI
    
    std::unique_ptr<ToolBarComponent> toolBar; ///< Toolbar UI
    
    std::unique_ptr<juce::Button> chooseFolderBtn; ///< Choose folder button
    
    std::unique_ptr<juce::ProgressBar> loadingFilesProgress; ///< Shows progress when parsing the music folder
    std::unique_ptr<AnalysisProgressBar> analysisProgress; ///< Shows progress when analysing the music library
    double loadingProgress = 0.0; ///< Stores the progress of the file parsing and analysis processes, to display in the progress bars (never both at the same time)
    
    juce::LookAndFeel_V4 customAppearance; ///< Stores the custom colour scheme
    juce::Colour colourBackground; ///< The background colour of the window (also stored in customAppearance, but here for easy access)
    
    juce::Image logo; ///< Logo image to display on the start screen
    juce::Rectangle<float> logoArea; ///< Area in which to display the start screen logo
    
    std::unique_ptr<AudioProcessor> audioProcessor; ///< Top-level audio processing object
    std::unique_ptr<DataManager> dataManager; ///< Manages all the track information in AutoDJ
    std::unique_ptr<ArtificialDJ> dj; ///< Decision-making DJ brain
    
    std::atomic<int> blockSize; ///< Size of the audio buffers to be processed
    
    std::atomic<bool> validSamplerate = false; ///< (Thread-safe) Indicates whether the current sample rate setting is valid
    std::atomic<bool> errorShown = false; ///< (Thread-safe) Indicates whether an error message has been shown after sample rate becomes invalid
    
    // State flags...
    bool startScreen = true; ///< Start screen flag: Indicates whether the app is on the start screen (i.e. a valid music folder has not been selected and parsed)
    bool loadingFiles = false; ///< Loading files flag: true when the files in the selected music folder are being parsed
    bool waitingForAnalysis = false; ///< Waiting for analysis flag: true when not all tracks in the library have been analysed
    bool ended = false; ///< Mix end flag: true when the DJ runs out of tracks to play and the mix comes to an end
    
#ifdef SHOW_GRAPH
    std::unique_ptr<juce::ResizableWindow> graphWindow; ///< Debugging graph to display data arrays, only if SHOW_GRAPH is defined
#endif
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent) ///< JUCE macro to add a memory leak detector
};
