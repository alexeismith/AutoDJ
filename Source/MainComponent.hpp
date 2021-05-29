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

//#define SHOW_GRAPH


class MainComponent : public juce::AudioAppComponent, public juce::Timer, public juce::Button::Listener
{
public:
    
    MainComponent();
    
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    
    void releaseResources() override;

    void paint(juce::Graphics& g) override;
    
    void resized() override;
    
    void timerCallback() override;
    
    void buttonClicked(juce::Button* button) override;
    
    bool validAudioSettings(bool showError = false);
    
    void changeView(ViewID view);

private:
    
    void resetMix();
    
    void setAppearance();
    
    void chooseFolder();
    
    juce::AudioDeviceManager customDeviceManager;
    
    std::unique_ptr<juce::AudioDeviceSelectorComponent> audioSettings;

    juce::LookAndFeel_V4 customAppearance;
    
    juce::ComponentBoundsConstrainer sizeLimits;
    
    std::unique_ptr<LibraryView> libraryView;
    std::unique_ptr<DirectionView> directionView;
    std::unique_ptr<MixView> mixView;
    
    std::unique_ptr<ToolBarComponent> toolBar;
    
    std::unique_ptr<juce::Button> chooseFolderBtn;
    
    std::unique_ptr<juce::ProgressBar> loadingFilesProgress;
    
    std::unique_ptr<AnalysisProgressBar> analysisProgress;
    
    juce::Colour colourBackground;
    
    juce::Image logo;
    juce::Rectangle<float> logoArea;
    
    std::unique_ptr<AudioProcessor> audioProcessor;
    std::unique_ptr<TrackDataManager> dataManager;
    std::unique_ptr<ArtificialDJ> dj;
    
    std::atomic<bool> validSamplerate = false;
    std::atomic<bool> errorShown = false;
    
    bool ended = false;
    
    std::atomic<int> initBlockSize;
    
    bool initialised = false;
    bool loadingFiles = false;
    bool waitingForAnalysis = false;
    
    double loadingProgress = 0.0;
    
#ifdef SHOW_GRAPH
    std::unique_ptr<juce::ResizableWindow> graphWindow;
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
