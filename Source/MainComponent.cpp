#include "MainComponent.hpp"

#include "CommonDefs.hpp"


MainComponent::MainComponent() :
    juce::AudioAppComponent(audioSettings) // Pass audio settings object to AudioAppComponent constructor
{
    // Initialise the audio hardware settings with 0 inputs and 2 outputs
    audioSettings.initialise(0, 2, nullptr, true);
    // Pass the audio settings object to the associated settings UI
    audioSettingsSelector.reset(new juce::AudioDeviceSelectorComponent(audioSettings, 0, 0, 2, 2, false, false, false, false));
    // Add the audio settings UI as a child of this component
    addChildComponent(audioSettingsSelector.get());

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
    
    // Set the app's colour scheme
    setAppearance();
    
    // Load the logo image to display on the start screen
    logo = juce::ImageFileFormat::loadFrom(BinaryData::logo_png, BinaryData::logo_pngSize);
    
    // Instantiate the track data manager
    dataManager.reset(new DataManager());
    
    // Instantiate the decision-making DJ brain, passing it the data manager
    dj.reset(new ArtificialDJ(dataManager.get()));
    
    // Instantiate the audio processor, passing it the data manager and DJ
    audioProcessor.reset(new AudioProcessor(dataManager.get(), dj.get(), blockSize.load()));
    
    // Pass the audio processor to the DJ
    dj->setAudioProcessor(audioProcessor.get());
    
    // Instantiate the Library view and add it as a child
    libraryView.reset(new LibraryView(dataManager.get()));
    addChildComponent(libraryView.get());
    
    // Instantiate the Direction view and add it as a child
    directionView.reset(new DirectionView(dataManager->getAnalysisManager()));
    addChildComponent(directionView.get());
    
    // Instantiate the Mix view and add it as a child
    mixView.reset(new MixView(audioProcessor->getTrackProcessors()));
    addChildComponent(mixView.get());
    
    // Instantiate the tool bar and add it as a child
    toolBar.reset(new ToolBarComponent(this, audioProcessor.get(), dj.get()));
    addChildComponent(toolBar.get());
    
    // Instantiate the choose folder button and add it as a visible child (shown on start screen)
    chooseFolderBtn.reset(new juce::TextButton("Choose Folder"));
    addAndMakeVisible(chooseFolderBtn.get());
    // Set the ID of the button, which indentifies it when clicked
    chooseFolderBtn->setComponentID(juce::String(ComponentID::chooseFolderBtn));
    // Add this component as a listener, to handle clicks of the button
    chooseFolderBtn->addListener(this);
    
    // Instantiate the file loading progress bar and add it as a child
    loadingFilesProgress.reset(new juce::ProgressBar(loadingProgress));
    addChildComponent(loadingFilesProgress.get());
    
    // Instantiate the audio analysis progress bar and add it as a child
    analysisProgress.reset(new AnalysisProgressBar(dataManager->getAnalysisManager()));
    addChildComponent(analysisProgress.get());
    
    // If the SHOW_GRAPH macro is defined...
#ifdef SHOW_GRAPH
    // Create a new window in which to display the debugging graph
    graphWindow.reset(new juce::ResizableWindow("Data Graph", true));
    // Set size and other basic properties
    graphWindow->setSize(1000, 200);
    graphWindow->setCentrePosition(400, 400);
    graphWindow->setUsingNativeTitleBar(true);
    graphWindow->setVisible(true);
    graphWindow->setResizable(true, true);
    // Pass it a new GraphComponent to display as its main UI component
    graphWindow->setContentOwned(new GraphComponent(), false);
#endif
    
    // Set the size of this main app window
    // Do this AFTER all the child components are created, so their resize() method is triggered (otherwise they will have 0 size)
    setSize (800, 550);
    
    // Set window resize limits
    sizeLimits.setSizeLimits(800, 475, 900, 650);
    
    // State the state refresh timer (see timerCallback())
    startTimerHz(30);
}


MainComponent::~MainComponent()
{
    // Destroy the views
    libraryView.reset();
    directionView.reset();
    mixView.reset();
    
    // Shut down the JUCE audio device
    shutdownAudio();
}


void MainComponent::resized()
{
    // Constrain the new size to the specified limits
    sizeLimits.checkComponentBounds(this);
    
    // Set the size and position of various child components,
    // based on the new size of this main window...
    
    toolBar->setSize(getWidth(), TOOLBAR_HEIGHT);
    toolBar->setTopLeftPosition(0, getHeight() - TOOLBAR_HEIGHT);
    
    audioSettingsSelector->setSize(getWidth() - 40, getHeight() - TOOLBAR_HEIGHT - 40);
    audioSettingsSelector->setCentrePosition(getWidth()/2 - 60, getHeight()/2 - TOOLBAR_HEIGHT);
    
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
    // The height of the analysis progress bar depends on which view is showing...
    if (directionView->isVisible())
        analysisProgress->setCentrePosition(getWidth()/2, getHeight() - TOOLBAR_HEIGHT - 30);
    else
        analysisProgress->setCentrePosition(getWidth()/2, getHeight() - TOOLBAR_HEIGHT - WAVEFORM_VIEW_HEIGHT - 30);
}


void MainComponent::paint (juce::Graphics& g)
{
    // If on the start screen, set a dark background gradient colour
    if (startScreen)
        g.setGradientFill(juce::ColourGradient(colourBackground, getWidth()/2, getHeight()/4, colourBackground.withBrightness(0.15f), getWidth(), getHeight(), true));
    else // Otherwise, show a lighter background gradient
        g.setGradientFill(juce::ColourGradient(colourBackground.withBrightness(0.3f), getWidth()/2, getHeight()/4, colourBackground, getWidth()/2, getHeight() - TOOLBAR_HEIGHT, true));
    // Fill the background with the chosen colour gradient
    g.fillAll();
    
    // If on the start screen, draw the AutoDJ logo
    if (startScreen)
        g.drawImage(logo, logoArea, juce::RectanglePlacement::centred);
}


void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // Store whether the supplied sample rate is valid
    if (sampleRate != SUPPORTED_SAMPLERATE)
    {
        validSamplerate.store(false);
    }
    else
    {
        validSamplerate.store(true);
        // If the sample rate is valid, reset the 'sample rate error shown' flag,
        // so that an error will be shown again if the sample rate becomes invalid in future
        errorShown.store(false);
    }
    
    // Store the expected audio processing buffer size
    blockSize.store(samplesPerBlockExpected);
    
    // If the audio processor has been instantiated, pass it the buffer size
    if (audioProcessor.get())
        audioProcessor->prepare(samplesPerBlockExpected);
}


void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& outputBuffer)
{
    // If the audio settings are invalid or the audio processor has not been instantiated, return
    if (!validAudioSettings() || audioProcessor.get() == nullptr) return;
    
    // Pass the audio output buffer to the audio processing object
    audioProcessor->getNextAudioBlock(outputBuffer);
}


void MainComponent::timerCallback()
{
    // If in a loading files state
    if (loadingFiles)
    {
        // Check the progress of the file parsing (loadingProgress and validDirectory are both output arguments here)
        if (!dataManager->isLoading(loadingProgress))
        {
            // If loading has completed...
            
            // Reset the laoding state flag
            loadingFiles = false;
            // Hide the progress bar
            loadingFilesProgress->setVisible(false);
            // Reset loading progress storage
            loadingProgress = 0.0;
            
            // If the directory was not valid, show an error message and return
            if (!dataManager->isDirectoryValid())
            {
                // Construct error message
                std::stringstream errorMessage;
                errorMessage << "Please choose a folder with at least " << NUM_TRACKS_MIN << " tracks. " \
                             << "These must be '.mp3' or '.wav' files, at least 60 seconds in length.";
                // Show an error window containing the message
                juce::AlertWindow::showMessageBox(juce::AlertWindow::WarningIcon, "Error", errorMessage.str(), "OK");
                // Show the choose folder button again
                chooseFolderBtn->setVisible(true);
                
                return;
            }
            // Otherwise, the directory was valid, so prepare the normal app state...
            
            // Set the start screen flag to false
            startScreen = false;
            
            // If analysis is underway...
            if (!dataManager->isAnalysisFinished())
            {
                // We are now waiting for audio analysis to complete
                // (this is automatically reset if there are no new files to analyse)
                waitingForAnalysis = true;
                // Show the analysis progress bar
                analysisProgress->setVisible(true);
            }
            
            if (dataManager->canStartPlaying())
            {
                toolBar->setCanPlay(true);
            }
            
            // Tell the Library view to load the files into its table
            libraryView->loadFiles();
            
            // Show the main tool bar
            toolBar->setVisible(true);
            
            // Show the audio hardware settings UI (underneath the other views)
            audioSettingsSelector->setVisible(true);
            // Show the library view
            changeView(ViewID::library);
            
            // Repaint the component
            repaint();
        }
    }
    // Or if waiting for files to be analysed
    else if (waitingForAnalysis)
    {
        // Get an analysis progress update from the data manager (loadingProgress and canStartPlaying are both output arguments here)
        if (dataManager->isAnalysisFinished(loadingProgress))
        {
            // If analysis has finished, reset the state flag and hide the progress bar
            waitingForAnalysis = false;
            analysisProgress->setVisible(false);
        }
        else
        {
            // Otherwise, update the progress bar
            analysisProgress->update(loadingProgress);
        }
        
        // If there are now enough analysed files, we can enable the play button
        if (dataManager->canStartPlaying())
            toolBar->setCanPlay(true);
    }
    
    // If the data manager has logged a track data update
    if (dataManager->trackDataUpdate.load())
    {
        // Reset the flag
        dataManager->trackDataUpdate.store(false);
        // Tell the Library and Direction view to update
        libraryView->refresh();
        directionView->refresh();
    }
    
    // If we are not on the start screen, and the chosen sample rate is invalid,
    // and an error has not yet been shown
    if (!startScreen && !validSamplerate.load() && !errorShown.load())
    {
        // Change to the audio settings
        toolBar->showSettings();
        
        // Log an error as shown - must do this before showing error so no more are shown
        errorShown.store(true);
        
        // Create error message about sample rate
        juce::String errorMessage = "Unsupported Sample Rate: please set to " + juce::String(SUPPORTED_SAMPLERATE) + "Hz.";
        // Show an error window containing the message
        juce::AlertWindow::showMessageBox(juce::AlertWindow::WarningIcon, "Error", errorMessage, "OK");
    }
    
    // If the audio processor says the mix has ended, and the flag here hasn't updated yet
    if (audioProcessor->mixEnded() && !ended)
    {
        // Update the flag
        ended = true;
        
        // Alert the user that there are no more tracks to play
        juce::AlertWindow::showMessageBox(juce::AlertWindow::InfoIcon, "Info", "Mix finished - ran out of analysed tracks.", "OK");
        
        // Reset the whole mix state ready to restart playback, if requested
        resetMix();
    }
}


void MainComponent::buttonClicked(juce::Button* button)
{
    // Fetch the ID of the button that was pressed
    int id = button->getComponentID().getIntValue();
    
    // React to the button press based on its ID
    // (Currently only the choose folder button in the component)
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
    // Currently audio setting validity only depends on sample rate
    bool valid = validSamplerate.load();
    
    // If the settings are not valid, and showError is supplied as true
    // Set the error message flag, to show an error on the next refresh
    if (!valid && showError)
        errorShown.store(false);
    
    // Return the result of the check
    return valid;
}


void MainComponent::resetMix()
{
    // Pause analysis
    analysisProgress->pause();
    
    // Reset the playing/played flags for all tracks
    dataManager->clearHistory();
    
    // Reset the DJ and audio processor
    dj->reset();
    audioProcessor->reset();
    
    // Un-pause analysis
    analysisProgress->playPause();
    
    // Reset the mix end flag
    ended = false;
}


void MainComponent::chooseFolder()
{
    // Create a JUCE file choosing object
    juce::FileChooser chooser ("Choose Music Folder");
    // Start the file chooser (opens a system dialog box)
    if (chooser.browseForDirectory())
    {
        // If the directory was selected...
        
        // Pass the directory to the data manager for parsing
        if (!dataManager->initialise(chooser.getResult(), directionView.get()))
            // If the data manager says the directory is not valid, return
            return;
        
        // Enter the loading files state
        loadingFiles = true;
        
        // Hide the choose folder button
        chooseFolderBtn->setVisible(false);
        // Show the loading files progress bar
        loadingFilesProgress->setVisible(true);
    }
}


void MainComponent::setAppearance()
{
    // Define the colour scheme to be used
    // The following colour values are an adaption of juce::LookAndFeel_V4::getMidnightColourScheme()
    juce::LookAndFeel_V4::ColourScheme colours = {
        0xff2f2f3a, juce::Colour(0xff191926).withBrightness(0.2f).withSaturation(0.2f), juce::Colour(0xffd0d0d0).brighter(),
        0xff66667c, juce::Colours::white, 0xffd8d8d8,
        0xffffffff, 0xff606073, 0xff000000 };
    // Pass the colour scheme to the JUCE look and feel object
    customAppearance.setColourScheme(colours);
    
    // Set a custom button colour
    customAppearance.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff191926).brighter());
    
    // Apply the custom look and feel
    juce::LookAndFeel::setDefaultLookAndFeel(&customAppearance);
    
    // Get a copy of the background colour, for easy access later
    colourBackground = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
}


void MainComponent::changeView(ViewID view)
{
    // Set all views invisible
    libraryView->setVisible(false);
    mixView->setVisible(false);
    directionView->setVisible(false);
    // Set the analysis progress bar invisible
    analysisProgress->setVisible(false);
    
    // Perform actions specific to the given view...
    switch (view)
    {
        case ViewID::library:
            // Show the Library view
            libraryView->setVisible(true);
            // If analysis is in progress, show the analysis progress bar
            if (waitingForAnalysis)
            {
                analysisProgress->setVisible(true);
                // The bar height depends on which view is showing
                analysisProgress->setCentrePosition(getWidth()/2, getHeight() - TOOLBAR_HEIGHT - WAVEFORM_VIEW_HEIGHT - 30);
            }
            break;
            
        case ViewID::direction:
            // Show the Direction view
            directionView->setVisible(true);
            // If analysis is in progress, show the analysis progress bar
            if (waitingForAnalysis)
            {
                analysisProgress->setVisible(true);
                // The bar height depends on which view is showing
                analysisProgress->setCentrePosition(getWidth()/2, getHeight() - TOOLBAR_HEIGHT - 30);
            }
            break;
            
        case ViewID::mix:
            // Show the Mix view
            mixView->setVisible(true);
            break;
            
        default:
            // Nothing to do for the settings view - it is underneath the other views, which are now hidden
            break;
    }
}

