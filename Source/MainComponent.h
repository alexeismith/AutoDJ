#pragma once

#include <JuceHeader.h>

#include "AudioProcessor.hpp"
#include "LibraryView.hpp"
#include "MixView.hpp"
#include "GraphComponent.hpp"
#include "ArtificialDJ.hpp"

//#define SHOW_GRAPH


class MainComponent : public juce::AudioAppComponent, public juce::Timer, public juce::Button::Listener
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

private:
    
    void setAppearance();
    
    std::atomic<int> initBlockSize;
    
    bool waitingForDJ = false;

    juce::LookAndFeel_V4 customAppearance;
    
    std::unique_ptr<AudioProcessor> audioProcessor;
    std::unique_ptr<TrackDataManager> dataManager;
    std::unique_ptr<ArtificialDJ> dj;
    
    std::unique_ptr<juce::Button> libraryBtn;
    std::unique_ptr<juce::Button> mixBtn;
    
    std::unique_ptr<juce::Button> playPauseBtn;
    
    std::unique_ptr<LibraryView> libraryView;

    std::unique_ptr<MixView> mixView;
    
#ifdef SHOW_GRAPH
    std::unique_ptr<juce::ResizableWindow> graphWindow;
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
