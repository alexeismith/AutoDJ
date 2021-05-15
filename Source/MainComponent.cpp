#include "MainComponent.h"

#include "CommonDefs.hpp"

MainComponent::MainComponent() :
    juce::AudioAppComponent(customDeviceManager)
{
    customDeviceManager.initialise(0, 2, nullptr, true);
    audioSettings.reset(new juce::AudioDeviceSelectorComponent(customDeviceManager, 0, 0, 2, 2, false, false, false, false));
    addChildComponent(audioSettings.get());
    
    setAppearance();
    
    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
    
    logo = juce::ImageFileFormat::loadFrom(BinaryData::logo_png, BinaryData::logo_pngSize);
    
    playImg = juce::ImageFileFormat::loadFrom(BinaryData::play_png, BinaryData::play_pngSize);
    pauseImg = juce::ImageFileFormat::loadFrom(BinaryData::pause_png, BinaryData::pause_pngSize);
    
    juce::Image skipImg = juce::ImageFileFormat::loadFrom(BinaryData::skip_png, BinaryData::skip_pngSize);
    juce::Image settingsImg = juce::ImageFileFormat::loadFrom(BinaryData::settings_png, BinaryData::settings_pngSize);
    
    volumeImg = juce::ImageFileFormat::loadFrom(BinaryData::volume_png, BinaryData::volume_pngSize);
    volumeImg.multiplyAllAlphas(0.8f);
    
    dataManager.reset(new TrackDataManager());
    dj.reset(new ArtificialDJ(dataManager.get()));
    
    audioProcessor.reset(new AudioProcessor(dataManager.get(), dj.get(), initBlockSize.load()));
    dj->setAudioProcessor(audioProcessor.get());
    
    libraryView.reset(new LibraryView(dataManager.get()));
    addChildComponent(libraryView.get());
    
    directionView.reset(new DirectionView(dataManager->getAnalysisManager()));
    addChildComponent(directionView.get());
    
    mixView.reset(new MixView(audioProcessor->getTrackProcessors()));
    addChildComponent(mixView.get());
    
    
    chooseFolderBtn.reset(new juce::TextButton("Choose Folder"));
    chooseFolderBtn->setComponentID(juce::String(ComponentID::chooseFolderBtn));
    addAndMakeVisible(chooseFolderBtn.get());
    chooseFolderBtn->addListener(this);
    
    loadingFilesProgress.reset(new juce::ProgressBar(loadingProgress));
    addChildComponent(loadingFilesProgress.get());
    
    libraryBtn.reset(new juce::TextButton("Library"));
    libraryBtn->setComponentID(juce::String(ComponentID::libraryBtn));
    addChildComponent(libraryBtn.get());
    libraryBtn->addListener(this);
    
    directionBtn.reset(new juce::TextButton("Direction"));
    directionBtn->setComponentID(juce::String(ComponentID::directionBtn));
    addChildComponent(directionBtn.get());
    directionBtn->addListener(this);
    
    mixBtn.reset(new juce::TextButton("Mix"));
    mixBtn->setComponentID(juce::String(ComponentID::mixBtn));
    addChildComponent(mixBtn.get());
    mixBtn->addListener(this);
    
    playPauseBtn.reset(new juce::ImageButton());
    addChildComponent(playPauseBtn.get());
    playPauseBtn->setImages(false, true, true, playImg, 0.8f, {}, playImg, 1.f, {}, playImg, 1.f, juce::Colours::lightblue);
    playPauseBtn->setComponentID(juce::String(ComponentID::playPauseBtn));
    playPauseBtn->addListener(this);
    playPauseBtn->setEnabled(false);
    
    skipBtn.reset(new juce::ImageButton());
    addChildComponent(skipBtn.get());
    skipBtn->setImages(false, true, true, skipImg, 0.8f, {}, skipImg, 1.f, {}, skipImg, 1.f, juce::Colours::lightblue);
    skipBtn->setComponentID(juce::String(ComponentID::skipBtn));
    skipBtn->addListener(this);
    skipBtn->setEnabled(false);
    
    volumeSld.reset(new juce::Slider());
    addChildComponent(volumeSld.get());
    volumeSld->setComponentID(juce::String(ComponentID::volumeSld));
    volumeSld->addListener(this);
    volumeSld->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    volumeSld->setRange(0.f, 1.f);
    volumeSld->setSkewFactor(0.7);
    volumeSld->setValue(1.f);
    
    settingsBtn.reset(new juce::ImageButton());
    addChildComponent(settingsBtn.get());
    settingsBtn->setImages(false, true, true, settingsImg, 0.8f, {}, settingsImg, 1.f, {}, settingsImg, 1.f, juce::Colours::lightblue);
    settingsBtn->setComponentID(juce::String(ComponentID::settingsBtn));
    settingsBtn->addListener(this);
    
    analysisProgress.reset(new AnalysisProgressBar(dataManager->getAnalysisManager()));
    addChildComponent(analysisProgress.get());
    
#ifdef SHOW_GRAPH
    graphWindow.reset(new juce::ResizableWindow("Data Graph", true));
    graphWindow->setSize(600, 600);
    graphWindow->setUsingNativeTitleBar(true);
    graphWindow->setCentrePosition(400, 400);
    graphWindow->setVisible(true);
    graphWindow->setResizable(true, true);
    graphWindow->setContentOwned(new GraphComponent(), false);
#endif
    
    // This needs to be done AFTER all the components are created, so that their resize() method can be called
    setSize (800, 550);
    
    // Set resize limits
    sizeLimits.setSizeLimits(800, 475, 900, 650);
    
    startTimerHz(30);
}

MainComponent::~MainComponent()
{
    libraryView.reset();
    directionView.reset();
    mixView.reset();
    
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    if (sampleRate != SUPPORTED_SAMPLERATE)
    {
        validSamplerate.store(false);
    }
    else
    {
        validSamplerate.store(true);
        errorShown.store(false);
    }
        
    if (audioProcessor.get())
        audioProcessor->prepare(samplesPerBlockExpected);
    else
        initBlockSize.store(samplesPerBlockExpected);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (!validSamplerate.load() || audioProcessor.get() == nullptr) return;
    
    audioProcessor->getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    if (!initialised)
        g.setGradientFill(juce::ColourGradient(colourBackground, getWidth()/2, getHeight()/4, colourBackground.withBrightness(0.15f), getWidth(), getHeight(), true));
    else
        g.setGradientFill(juce::ColourGradient(colourBackground.withBrightness(0.3f), getWidth()/2, getHeight()/4, colourBackground, getWidth()/2, getHeight() - TOOLBAR_HEIGHT, true));
    g.fillAll();
    
    if (!initialised)
        g.drawImage(logo, logoArea, juce::RectanglePlacement::centred);
    else
        g.drawImage(volumeImg, volumeArea, juce::RectanglePlacement::centred);
}

void MainComponent::resized()
{
    sizeLimits.checkComponentBounds(this);
    
    audioSettings->setSize(getWidth() - 40, getHeight() - TOOLBAR_HEIGHT - 40);
    audioSettings->setCentrePosition(getWidth()/2 - 60, getHeight()/2 - TOOLBAR_HEIGHT);
    
    logoArea.setSize(220, 121);
    logoArea.setCentre(getWidth()/2, getHeight()/2 - 35);
    
    chooseFolderBtn->setSize(120, 35);
    chooseFolderBtn->setCentrePosition(getWidth()/2, getHeight()/2 + 60);
    
    loadingFilesProgress->setSize(150, 30);
    loadingFilesProgress->setCentrePosition(getWidth()/2, getHeight()/2 + 60);
    
    libraryView->setSize(getWidth(), getHeight() - TOOLBAR_HEIGHT);
    libraryView->setTopLeftPosition(0, 0);
    
    directionView->setSize(getWidth(), getHeight() - TOOLBAR_HEIGHT);
    directionView->setTopLeftPosition(0, 0);
    
    mixView->setSize(getWidth(), getHeight() - TOOLBAR_HEIGHT);
    mixView->setTopLeftPosition(0, 0);
    
    libraryBtn->setSize(68, 28);
    libraryBtn->setCentrePosition(libraryBtn->getWidth()/2 + 10, getHeight() - TOOLBAR_HEIGHT/2);
    
    directionBtn->setSize(80, 28);
    directionBtn->setCentrePosition(directionBtn->getWidth()/2 + 88, getHeight() - TOOLBAR_HEIGHT/2);
    
    mixBtn->setSize(50, 28);
    mixBtn->setCentrePosition(mixBtn->getWidth()/2 + 178, getHeight() - TOOLBAR_HEIGHT/2);
    
    playPauseBtn->setSize(24, 24);
    playPauseBtn->setCentrePosition(getWidth()/2, getHeight() - TOOLBAR_HEIGHT/2);
    
    skipBtn->setSize(25, 25);
    skipBtn->setCentrePosition(getWidth()/2 + 40, getHeight() - TOOLBAR_HEIGHT/2);
    
    volumeSld->setSize(140, 50);
    volumeSld->setCentrePosition(getWidth() - 125, getHeight() - TOOLBAR_HEIGHT/2);
    
    volumeArea.setSize(22, 22);
    volumeArea.setCentre(volumeSld->getX() - 10, getHeight() - TOOLBAR_HEIGHT/2);
    
    settingsBtn->setSize(25, 25);
    settingsBtn->setCentrePosition(getWidth() - 25, getHeight() - TOOLBAR_HEIGHT/2);
    
    analysisProgress->setSize(300, 30);
    if (directionView->isVisible())
        analysisProgress->setCentrePosition(getWidth()/2, getHeight() - TOOLBAR_HEIGHT - 30);
    else
        analysisProgress->setCentrePosition(getWidth()/2, getHeight() - TOOLBAR_HEIGHT - WAVEFORM_VIEW_HEIGHT - 30);
}


void MainComponent::timerCallback()
{
    if (loadingFiles)
    {
        bool validDirectory;
        
        if (!dataManager->isLoading(loadingProgress, validDirectory))
        {
            loadingFiles = false;
            loadingFilesProgress->setVisible(false);
            loadingProgress = 0.0;
            
            if (!validDirectory)
            {
                std::stringstream errorMessage;
                errorMessage << "Please choose a folder with at least " << NUM_TRACKS_MIN << " tracks. " \
                             << "These must be '.mp3' or '.wav' files, at least 60 seconds in length.";
                // Show an error window containing the message
                juce::AlertWindow::showMessageBox(juce::AlertWindow::WarningIcon, "Error", errorMessage.str(), "OK");
                
                chooseFolderBtn->setVisible(true);
                return;
            }
            
            initialised = true;
            
            waitingForAnalysis = true;
            
            libraryView->loadFiles();
            
            audioSettings->setVisible(true);
            libraryBtn->setVisible(true);
            directionBtn->setVisible(true);
            mixBtn->setVisible(true);
            volumeSld->setVisible(true);
            playPauseBtn->setVisible(true);
            skipBtn->setVisible(true);
            settingsBtn->setVisible(true);
            analysisProgress->setVisible(true);
            
            changeView(ViewID::library);
            
            repaint();
        }
    }
    
    if (waitingForAnalysis)
    {
        bool canStartPlaying;
        
        analysisProgress->update(loadingProgress);
        
        if (dataManager->analysisProgress(loadingProgress, canStartPlaying))
        {
            waitingForAnalysis = false;
            analysisProgress->setVisible(false);
        }
        
        if (canStartPlaying)
            playPauseBtn->setEnabled(true);
    }
    
    if (waitingForDJ)
    {
        if (dj->isInitialised())
        {
            waitingForDJ = false;
            playing = true;
            playPauseBtn->setAlpha(1.f);
            playPauseBtn->setImages(false, true, true, pauseImg, 0.8f, {}, pauseImg, 1.f, {}, pauseImg, 1.f, juce::Colours::lightblue);
            skipBtn->setEnabled(true);
        }
        else
        {
            juce::uint32 seconds = juce::Time::getApproximateMillisecondCounter();
            double sine = (std::sin(double(seconds) / 150) + 1.0) / 3.0;
            playPauseBtn->setAlpha(sine + 0.3f);
        }
    }
    
    if (dataManager->trackDataUpdate.load())
    {
        dataManager->trackDataUpdate.store(false);
        libraryView->updateData();
        directionView->updateData();
    }
    
    if (!loadingFiles && !validSamplerate.load() && !errorShown.load())
    {
        errorShown.store(true);
        
        // Create error message about sample rate
        juce::String errorMessage = "Unsupported Sample Rate: please set to " + juce::String(SUPPORTED_SAMPLERATE) + "Hz.";
        
        if (currentView != ViewID::settings)
            changeView(ViewID::settings);
        
        // Show an error window containing the message
        juce::AlertWindow::showMessageBox(juce::AlertWindow::WarningIcon, "Error", errorMessage, "OK");
    }
    
    if (audioProcessor->mixEnded() && !ended)
    {
        playPauseBtn->setImages(false, true, true, playImg, 0.8f, {}, playImg, 1.f, {}, playImg, 1.f, juce::Colours::lightblue);
        playing = false;
        
        ended = true;
        
        juce::AlertWindow::showMessageBox(juce::AlertWindow::InfoIcon, "Info", "Mix finished - ran out of analysed tracks.", "OK");
        
        resetMix();
    }
    
    skipBtn->setEnabled(dj->canSkip() && validSamplerate.load());
}


void MainComponent::buttonClicked(juce::Button* button)
{
    int id = button->getComponentID().getIntValue();
    
    switch (id)
    {
        case ComponentID::chooseFolderBtn:
            chooseFolder();
            break;
            
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
            if (!validSamplerate.load())
            {
                errorShown.store(false);
                break;
            }
            
            if (waitingForDJ)
                break;
            
            if (!dj->playPause())
                waitingForDJ = true;
            else
            {
                playing = !playing;
                
                if (playing)
                    playPauseBtn->setImages(false, true, true, pauseImg, 0.8f, {}, pauseImg, 1.f, {}, pauseImg, 1.f, juce::Colours::lightblue);
                else
                    playPauseBtn->setImages(false, true, true, playImg, 0.8f, {}, playImg, 1.f, {}, playImg, 1.f, juce::Colours::lightblue);
            }
            break;
            
        case ComponentID::skipBtn:
            if (dj->canSkip())
                audioProcessor->skip();
            break;

        case ComponentID::settingsBtn:
            changeView(ViewID::settings);
            break;
            
        default:
            jassert(false); // Unrecognised button ID
    }
}


void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    int id = slider->getComponentID().getIntValue();
    
    switch (id)
    {
        case ComponentID::volumeSld:
            audioProcessor->setVolume(slider->getValueObject().getValue());
            break;
            
        default:
            jassert(false); // Unrecognised button ID
    }
}


void MainComponent::resetMix()
{
    analysisProgress->pause();
    
    dataManager->clearHistory();
    dj->reset();
    audioProcessor->reset();
    
    analysisProgress->playPause();
    
    ended = false;
}


void MainComponent::chooseFolder()
{
    juce::FileChooser chooser ("Choose Music Folder");
    if (chooser.browseForDirectory())
    {
        if (!dataManager->initialise(chooser.getResult(), directionView.get()))
            return;
        
        loadingFiles = true;
        
        chooseFolderBtn->setVisible(false);
        loadingFilesProgress->setVisible(true);
    }
}


void MainComponent::setAppearance()
{
    // The following colour values are an adaption of juce::LookAndFeel_V4::getMidnightColourScheme()
    
    juce::LookAndFeel_V4::ColourScheme colours = {
        0xff2f2f3a, juce::Colour(0xff191926).withBrightness(0.2f).withSaturation(0.2f), juce::Colour(0xffd0d0d0).brighter(),
        0xff66667c, juce::Colours::white, 0xffd8d8d8,
        0xffffffff, 0xff606073, 0xff000000 };
    customAppearance.setColourScheme(colours);
    
    customAppearance.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff191926).brighter());
    
    juce::LookAndFeel::setDefaultLookAndFeel(&customAppearance);
    
    colourBackground = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
}


void MainComponent::changeView(ViewID view)
{
    libraryView->setVisible(false);
    mixView->setVisible(false);
    directionView->setVisible(false);
    analysisProgress->setVisible(false);
    libraryBtn->setColour(juce::TextButton::ColourIds::buttonColourId, colourBackground.withBrightness(0.24f));
    directionBtn->setColour(juce::TextButton::ColourIds::buttonColourId, colourBackground.withBrightness(0.24f));
    mixBtn->setColour(juce::TextButton::ColourIds::buttonColourId, colourBackground.withBrightness(0.24f));
    
    switch (view)
    {
        case ViewID::library:
            libraryView->setVisible(true);
            libraryBtn->setColour(juce::TextButton::ColourIds::buttonColourId, customAppearance.findColour(juce::TextButton::ColourIds::buttonOnColourId));
            if (waitingForAnalysis)
            {
                analysisProgress->setVisible(true);
                analysisProgress->setCentrePosition(getWidth()/2, getHeight() - TOOLBAR_HEIGHT - WAVEFORM_VIEW_HEIGHT - 30);
            }
            break;
            
        case ViewID::direction:
            directionView->setVisible(true);
            directionBtn->setColour(juce::TextButton::ColourIds::buttonColourId, customAppearance.findColour(juce::TextButton::ColourIds::buttonOnColourId));
            if (waitingForAnalysis)
            {
                analysisProgress->setVisible(true);
                analysisProgress->setCentrePosition(getWidth()/2, getHeight() - TOOLBAR_HEIGHT - 30);
            }
            break;
            
        case ViewID::mix:
            mixView->setVisible(true);
            mixBtn->setColour(juce::TextButton::ColourIds::buttonColourId, customAppearance.findColour(juce::TextButton::ColourIds::buttonOnColourId));
            break;
            
        case ViewID::settings:
            if (currentView == ViewID::settings)
            {
                changeView(prevView);
                return;
            }
    }
    
    prevView = currentView;
    currentView = view;
}

