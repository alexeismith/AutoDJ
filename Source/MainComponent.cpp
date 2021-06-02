#include "MainComponent.hpp"

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
    
    dataManager.reset(new DataManager());
    dj.reset(new ArtificialDJ(dataManager.get()));
    
    audioProcessor.reset(new AudioProcessor(dataManager.get(), dj.get(), initBlockSize.load()));
    dj->setAudioProcessor(audioProcessor.get());
    
    libraryView.reset(new LibraryView(dataManager.get()));
    addChildComponent(libraryView.get());
    
    directionView.reset(new DirectionView(dataManager->getAnalysisManager()));
    addChildComponent(directionView.get());
    
    mixView.reset(new MixView(audioProcessor->getTrackProcessors()));
    addChildComponent(mixView.get());
    
    toolBar.reset(new ToolBarComponent(this, audioProcessor.get(), dj.get()));
    addChildComponent(toolBar.get());
    
    chooseFolderBtn.reset(new juce::TextButton("Choose Folder"));
    chooseFolderBtn->setComponentID(juce::String(ComponentID::chooseFolderBtn));
    addAndMakeVisible(chooseFolderBtn.get());
    chooseFolderBtn->addListener(this);
    
    loadingFilesProgress.reset(new juce::ProgressBar(loadingProgress));
    addChildComponent(loadingFilesProgress.get());
    
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
    if (!validAudioSettings() || audioProcessor.get() == nullptr) return;
    
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
}

void MainComponent::resized()
{
    sizeLimits.checkComponentBounds(this);
    
    toolBar->setSize(getWidth(), TOOLBAR_HEIGHT);
    toolBar->setTopLeftPosition(0, getHeight() - TOOLBAR_HEIGHT);
    
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
            toolBar->setVisible(true);
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
            toolBar->setCanPlay(true);
    }
    
    if (dataManager->trackDataUpdate.load())
    {
        dataManager->trackDataUpdate.store(false);
        libraryView->refreshTable();
        directionView->updateData();
    }
    
    if (initialised && !validSamplerate.load() && !errorShown.load())
    {
        errorShown.store(true);
        
        // Create error message about sample rate
        juce::String errorMessage = "Unsupported Sample Rate: please set to " + juce::String(SUPPORTED_SAMPLERATE) + "Hz.";
        
        toolBar->showSettings();
        
        // Show an error window containing the message
        juce::AlertWindow::showMessageBox(juce::AlertWindow::WarningIcon, "Error", errorMessage, "OK");
    }
    
    if (audioProcessor->mixEnded() && !ended)
    {
        ended = true;
        
        juce::AlertWindow::showMessageBox(juce::AlertWindow::InfoIcon, "Info", "Mix finished - ran out of analysed tracks.", "OK");
        
        resetMix();
    }
}


void MainComponent::buttonClicked(juce::Button* button)
{
    int id = button->getComponentID().getIntValue();
    
    switch (id)
    {
        case ComponentID::chooseFolderBtn:
            chooseFolder();
            break;
            
        default:
            jassert(false); // Unrecognised button ID
    }
}


bool MainComponent::validAudioSettings(bool showError)
{
    bool valid = validSamplerate.load();
    
    if (!valid && showError)
        errorShown.store(false);
    
    return valid;
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
    
    switch (view)
    {
        case ViewID::library:
            libraryView->setVisible(true);
            if (waitingForAnalysis)
            {
                analysisProgress->setVisible(true);
                analysisProgress->setCentrePosition(getWidth()/2, getHeight() - TOOLBAR_HEIGHT - WAVEFORM_VIEW_HEIGHT - 30);
            }
            break;
            
        case ViewID::direction:
            directionView->setVisible(true);
            if (waitingForAnalysis)
            {
                analysisProgress->setVisible(true);
                analysisProgress->setCentrePosition(getWidth()/2, getHeight() - TOOLBAR_HEIGHT - 30);
            }
            break;
            
        case ViewID::mix:
            mixView->setVisible(true);
            break;
            
        default:
            break;
    }
}

