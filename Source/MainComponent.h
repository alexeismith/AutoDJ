#pragma once

#include <JuceHeader.h>

#include "LibraryComponent.hpp"


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

    std::unique_ptr<LibraryComponent> library;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
