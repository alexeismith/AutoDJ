//
//  ToolBarComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 29/05/2021.
//

#include "ToolBarComponent.hpp"

#include "MainComponent.hpp"

ToolBarComponent::ToolBarComponent(juce::Component* mainComp, AudioProcessor* ap, ArtificialDJ* DJ) :
    mainComponent(mainComp), audioProcessor(ap), dj(DJ)
{
    playImg = juce::ImageFileFormat::loadFrom(BinaryData::play_png, BinaryData::play_pngSize);
    pauseImg = juce::ImageFileFormat::loadFrom(BinaryData::pause_png, BinaryData::pause_pngSize);
    
    juce::Image skipImg = juce::ImageFileFormat::loadFrom(BinaryData::skip_png, BinaryData::skip_pngSize);
    juce::Image settingsImg = juce::ImageFileFormat::loadFrom(BinaryData::settings_png, BinaryData::settings_pngSize);
    
    volumeImg = juce::ImageFileFormat::loadFrom(BinaryData::volume_png, BinaryData::volume_pngSize);
    volumeImg.multiplyAllAlphas(0.8f);
    
    libraryBtn.reset(new juce::TextButton("Library"));
    libraryBtn->setComponentID(juce::String(ComponentID::libraryBtn));
    addAndMakeVisible(libraryBtn.get());
    libraryBtn->addListener(this);
    libraryBtn->setColour(juce::TextButton::ColourIds::buttonColourId, getLookAndFeel().findColour(juce::TextButton::ColourIds::buttonOnColourId));
    
    directionBtn.reset(new juce::TextButton("Direction"));
    directionBtn->setComponentID(juce::String(ComponentID::directionBtn));
    addAndMakeVisible(directionBtn.get());
    directionBtn->addListener(this);
    directionBtn->setColour(juce::TextButton::ColourIds::buttonColourId, colourBackground.withBrightness(0.24f));
    
    mixBtn.reset(new juce::TextButton("Mix"));
    mixBtn->setComponentID(juce::String(ComponentID::mixBtn));
    addAndMakeVisible(mixBtn.get());
    mixBtn->addListener(this);
    mixBtn->setColour(juce::TextButton::ColourIds::buttonColourId, colourBackground.withBrightness(0.24f));
    
    playPauseBtn.reset(new juce::ImageButton());
    addAndMakeVisible(playPauseBtn.get());
    playPauseBtn->setImages(false, true, true, playImg, 0.8f, {}, playImg, 1.f, {}, playImg, 1.f, juce::Colours::lightblue);
    playPauseBtn->setComponentID(juce::String(ComponentID::playPauseBtn));
    playPauseBtn->addListener(this);
    playPauseBtn->setEnabled(false);
    
    skipBtn.reset(new juce::ImageButton());
    addAndMakeVisible(skipBtn.get());
    skipBtn->setImages(false, true, true, skipImg, 0.8f, {}, skipImg, 1.f, {}, skipImg, 1.f, juce::Colours::lightblue);
    skipBtn->setComponentID(juce::String(ComponentID::skipBtn));
    skipBtn->addListener(this);
    skipBtn->setEnabled(false);
    
    volumeSld.reset(new juce::Slider());
    addAndMakeVisible(volumeSld.get());
    volumeSld->setComponentID(juce::String(ComponentID::volumeSld));
    volumeSld->addListener(this);
    volumeSld->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    volumeSld->setRange(0.f, 1.f);
    volumeSld->setSkewFactor(0.7);
    volumeSld->setValue(1.f);
    
    settingsBtn.reset(new juce::ImageButton());
    addAndMakeVisible(settingsBtn.get());
    settingsBtn->setImages(false, true, true, settingsImg, 0.8f, {}, settingsImg, 1.f, {}, settingsImg, 1.f, juce::Colours::lightblue);
    settingsBtn->setComponentID(juce::String(ComponentID::settingsBtn));
    settingsBtn->addListener(this);
    
    colourBackground = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
    
    startTimerHz(30);
}


void ToolBarComponent::resized()
{
    libraryBtn->setSize(68, 28);
    libraryBtn->setCentrePosition(libraryBtn->getWidth()/2 + 10, getHeight()/2);
    
    directionBtn->setSize(80, 28);
    directionBtn->setCentrePosition(directionBtn->getWidth()/2 + 88, getHeight()/2);
    
    mixBtn->setSize(50, 28);
    mixBtn->setCentrePosition(mixBtn->getWidth()/2 + 178, getHeight()/2);
    
    playPauseBtn->setSize(24, 24);
    playPauseBtn->setCentrePosition(getWidth()/2, getHeight()/2);
    
    skipBtn->setSize(25, 25);
    skipBtn->setCentrePosition(getWidth()/2 + 40, getHeight()/2);
    
    volumeSld->setSize(140, 50);
    volumeSld->setCentrePosition(getWidth() - 125, getHeight()/2);
    
    volumeArea.setSize(22, 22);
    volumeArea.setCentre(volumeSld->getX() - 10, getHeight()/2);
    
    settingsBtn->setSize(25, 25);
    settingsBtn->setCentrePosition(getWidth() - 25, getHeight()/2);
}


void ToolBarComponent::paint(juce::Graphics& g)
{
    g.drawImage(volumeImg, volumeArea, juce::RectanglePlacement::centred);
}


void ToolBarComponent::timerCallback()
{
    // If we are waiting for the DJ to initialise the mix queue
    if (waitingForDJ)
    {
        // Check wether the mix queue is now initialised
        if (dj->isInitialised())
        {
            // If initialised, the mix is now playing...
            
            waitingForDJ = false;
            playing = true;
            playPauseBtn->setAlpha(1.f);
            playPauseBtn->setImages(false, true, true, pauseImg, 0.8f, {}, pauseImg, 1.f, {}, pauseImg, 1.f, juce::Colours::lightblue);
            skipBtn->setEnabled(true);
        }
        else
        {
            // If the DJ is still thinking, reflect this by animating the play button...
            
            // Use the current time to generate a sine signal
            juce::uint32 seconds = juce::Time::getApproximateMillisecondCounter();
            double sine = (std::sin(double(seconds) / 150) + 1.0) / 3.0;
            // Apply the sine signal to the transparency of the play button
            playPauseBtn->setAlpha(sine + 0.3f);
        }
    }
    
    // If the audio processor says the mix has finished, and we haven't reacted to it here
    if (audioProcessor->mixEnded() && !ended)
    {
        // Set playback at paused
        setPlayPause(false);
        // Set the mix end flag
        ended = true;
    }
    
    // If the DJ is ready, and the audio hardware settings are valid, enable the skip button
    if (dj->canSkip() && ((MainComponent*)mainComponent)->validAudioSettings())
        skipBtn->setEnabled(true);
    else // Otherwise, disable it
        skipBtn->setEnabled(false);
}


void ToolBarComponent::buttonClicked(juce::Button* button)
{
    int id = button->getComponentID().getIntValue();
    
    switch (id)
    {
        case ComponentID::libraryBtn:
            changeView(ViewID::library);
            break;
            
        case ComponentID::directionBtn:
            changeView(ViewID::direction);
            break;
            
        case ComponentID::mixBtn:
            changeView(ViewID::mix);
            break;
            
        case ComponentID::playPauseBtn:
            if (!((MainComponent*)mainComponent)->validAudioSettings(true))
                break;
            
            if (waitingForDJ)
                break;
            
            if (!dj->playPause())
                waitingForDJ = true;
            else
            {
                setPlayPause(!playing);
            }
            break;
            
        case ComponentID::skipBtn:
            if (dj->canSkip())
                audioProcessor->skip();
            break;

        case ComponentID::settingsBtn:
            // If the settings view is not already showing, show it
            if (currentView != ViewID::settings)
                changeView(ViewID::settings);
            else // Otherwise, show the previous view
                changeView(prevView);
            break;
            
        default:
            jassert(false); // Unrecognised button ID
    }
}


void ToolBarComponent::sliderValueChanged(juce::Slider* slider)
{
    int id = slider->getComponentID().getIntValue();
    
    switch (id)
    {
        case ComponentID::volumeSld:
            audioProcessor->setVolume(slider->getValueObject().getValue());
            break;
            
        default:
            jassert(false); // Unrecognised slider ID
    }
}


void ToolBarComponent::setPlayPause(bool play)
{
    playing = play;
    
    if (playing)
        playPauseBtn->setImages(false, true, true, pauseImg, 0.8f, {}, pauseImg, 1.f, {}, pauseImg, 1.f, juce::Colours::lightblue);
    else
        playPauseBtn->setImages(false, true, true, playImg, 0.8f, {}, playImg, 1.f, {}, playImg, 1.f, juce::Colours::lightblue);
}


void ToolBarComponent::changeView(ViewID view)
{
    // First, set all the tab buttons to the 'unselected' colour
    libraryBtn->setColour(juce::TextButton::ColourIds::buttonColourId, colourBackground.withBrightness(0.24f));
    directionBtn->setColour(juce::TextButton::ColourIds::buttonColourId, colourBackground.withBrightness(0.24f));
    mixBtn->setColour(juce::TextButton::ColourIds::buttonColourId, colourBackground.withBrightness(0.24f));
    
    // Tell the main UI to change view
    ((MainComponent*)mainComponent)->changeView(view);
    
    // Set the relevant tab button to the 'selected' colour
    // (All are already unselected if we are changing to the settings view)
    switch (view)
    {
        case ViewID::library:
            libraryBtn->setColour(juce::TextButton::ColourIds::buttonColourId, getLookAndFeel().findColour(juce::TextButton::ColourIds::buttonOnColourId));
            break;
            
        case ViewID::direction:
            directionBtn->setColour(juce::TextButton::ColourIds::buttonColourId, getLookAndFeel().findColour(juce::TextButton::ColourIds::buttonOnColourId));
            break;
            
        case ViewID::mix:
            mixBtn->setColour(juce::TextButton::ColourIds::buttonColourId, getLookAndFeel().findColour(juce::TextButton::ColourIds::buttonOnColourId));
            break;
            
        default:
            break;
    }
    
    // Update the current and previous view trackers
    prevView = currentView;
    currentView = view;
}


void ToolBarComponent::showSettings()
{
    if (currentView != ViewID::settings)
        changeView(ViewID::settings);
}
