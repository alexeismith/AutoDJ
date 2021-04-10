#pragma once

#include <JuceHeader.h>

#include "AudioProcessor.hpp"
#include "LibraryComponent.hpp"
#include "GraphComponent.hpp"

//#define SHOW_GRAPH


class MainComponent  : public juce::AudioAppComponent
{
public:
    
    MainComponent();
    
    ~MainComponent() override;

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    
    void releaseResources() override;

    void paint (juce::Graphics& g) override;
    
    void resized() override;

private:
    
    void setAppearance();

    juce::LookAndFeel_V4 customAppearance;
    
    std::unique_ptr<AudioProcessor> audioProcessor;
    
    std::unique_ptr<LibraryComponent> library;
    
#ifdef SHOW_GRAPH
    std::unique_ptr<juce::ResizableWindow> graphWindow;
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
