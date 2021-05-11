#pragma once

#include <JuceHeader.h>

#include "AudioProcessor.hpp"
#include "LibraryView.hpp"
#include "DirectionView.hpp"
#include "MixView.hpp"
#include "GraphComponent.hpp"
#include "ArtificialDJ.hpp"
#include "AnalysisProgressBar.hpp"

//#define SHOW_GRAPH

enum ViewID : int
{
    library,
    direction,
    mix,
    settings
};


class MainComponent : public juce::AudioAppComponent, public juce::Timer, public juce::Button::Listener, public juce::Slider::Listener
{
public:
    
    MainComponent();
    
    ~MainComponent() override;

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    
    void releaseResources() override;

    void paint (juce::Graphics& g) override;
    
    void resized() override;
    
    void timerCallback() override;
    
    void buttonClicked(juce::Button* button) override;
    
    void sliderValueChanged(juce::Slider* slider) override;

private:
    
    void resetMix();
    
    void setAppearance();
    
    void chooseFolder();
    
    void changeView(ViewID view);
    
    juce::AudioDeviceManager customDeviceManager;
    
    std::unique_ptr<juce::AudioDeviceSelectorComponent> audioSettings;

    juce::LookAndFeel_V4 customAppearance;
    
    juce::ComponentBoundsConstrainer sizeLimits;
    
    std::unique_ptr<LibraryView> libraryView;
    std::unique_ptr<DirectionView> directionView;
    std::unique_ptr<MixView> mixView;
    
    std::unique_ptr<juce::Button> chooseFolderBtn;
    
    std::unique_ptr<juce::ProgressBar> loadingFilesProgress;
    
    std::unique_ptr<AnalysisProgressBar> analysisProgress;

    std::unique_ptr<juce::Button> libraryBtn;
    std::unique_ptr<juce::Button> directionBtn;
    std::unique_ptr<juce::Button> mixBtn;
    
    std::unique_ptr<juce::ImageButton> playPauseBtn;
    std::unique_ptr<juce::ImageButton> skipBtn;
    std::unique_ptr<juce::ImageButton> settingsBtn;
    
    std::unique_ptr<juce::Slider> volumeSld;
    
    juce::Colour colourBackground;
    
    juce::Image logo;
    juce::Rectangle<float> logoArea;
    
    juce::Image playImg;
    juce::Image pauseImg;
    
    juce::Image volumeImg;
    juce::Rectangle<float> volumeArea;
    
    ViewID currentView = ViewID::library, prevView = ViewID::library;
    
    std::unique_ptr<AudioProcessor> audioProcessor;
    std::unique_ptr<TrackDataManager> dataManager;
    std::unique_ptr<ArtificialDJ> dj;
    
    std::atomic<bool> validSamplerate = false;
    std::atomic<bool> errorShown = false;
    
    bool playing = false;
    bool ended = false;
    
    std::atomic<int> initBlockSize;
    
    bool initialised = false;
    bool loadingFiles = false;
    bool waitingForAnalysis = false;
    bool waitingForDJ = false;
    
    double loadingProgress = 0.0;
    
#ifdef SHOW_GRAPH
    std::unique_ptr<juce::ResizableWindow> graphWindow;
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
