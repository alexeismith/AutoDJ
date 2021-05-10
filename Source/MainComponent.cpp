#include "MainComponent.h"

#include "CommonDefs.hpp"

MainComponent::MainComponent()
{
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
    chooseFolderBtn->setComponentID(juce::String(ComponentIDs::chooseFolderBtn));
    addAndMakeVisible(chooseFolderBtn.get());
    chooseFolderBtn->addListener(this);
    
    loadingFilesProgress.reset(new juce::ProgressBar(loadingProgress));
    addChildComponent(loadingFilesProgress.get());
    
    libraryBtn.reset(new juce::TextButton("Library"));
    libraryBtn->setComponentID(juce::String(ComponentIDs::libraryBtn));
    addChildComponent(libraryBtn.get());
    libraryBtn->addListener(this);
    
    directionBtn.reset(new juce::TextButton("Direction"));
    directionBtn->setComponentID(juce::String(ComponentIDs::directionBtn));
    addChildComponent(directionBtn.get());
    directionBtn->addListener(this);
    
    mixBtn.reset(new juce::TextButton("Mix"));
    mixBtn->setComponentID(juce::String(ComponentIDs::mixBtn));
    addChildComponent(mixBtn.get());
    mixBtn->addListener(this);
    
    playImg = juce::ImageFileFormat::loadFrom(BinaryData::play_png, BinaryData::play_pngSize);
    pauseImg = juce::ImageFileFormat::loadFrom(BinaryData::pause_png, BinaryData::pause_pngSize);
    
    playPauseBtn.reset(new juce::ImageButton());
    addChildComponent(playPauseBtn.get());
    playPauseBtn->setImages(false, true, true, playImg, 0.8f, {}, playImg, 1.f, {}, playImg, 1.f, juce::Colours::lightblue);
    playPauseBtn->setComponentID(juce::String(ComponentIDs::playPauseBtn));
    playPauseBtn->addListener(this);
    playPauseBtn->setEnabled(false);
    
    juce::Image skipImg = juce::ImageFileFormat::loadFrom(BinaryData::skip_png, BinaryData::skip_pngSize);
    
    skipBtn.reset(new juce::ImageButton());
    addChildComponent(skipBtn.get());
    skipBtn->setImages(false, true, true, skipImg, 0.8f, {}, skipImg, 1.f, {}, skipImg, 1.f, juce::Colours::lightblue);
    skipBtn->setComponentID(juce::String(ComponentIDs::skipBtn));
    skipBtn->addListener(this);
    skipBtn->setEnabled(false);
    
    volumeSld.reset(new juce::Slider());
    addChildComponent(volumeSld.get());
    volumeSld->setComponentID(juce::String(ComponentIDs::volumeSld));
    volumeSld->addListener(this);
    volumeSld->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    volumeSld->setRange(0.f, 1.f);
    volumeSld->setSkewFactor(0.7);
    volumeSld->setValue(1.f);
    
    analysisProgress.reset(new AnalysisProgressBar(dataManager->getAnalysisManager()));
    addChildComponent(analysisProgress.get());
    analysisProgress->setColour(analysisProgress->backgroundColourId, juce::Colours::darkgrey);
    
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
    sizeLimits.setSizeLimits(800, 475, 1200, 700);
    
    startTimerHz(30);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
    
    if (audioProcessor.get())
        audioProcessor->prepare(samplesPerBlockExpected);
    else
        initBlockSize.store(samplesPerBlockExpected);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (audioProcessor.get() == nullptr) return;
    
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
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.drawImage(logo, logoArea, juce::RectanglePlacement::centred);
}

void MainComponent::resized()
{
    sizeLimits.checkComponentBounds(this);
    
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
    volumeSld->setCentrePosition(getWidth() - 80, getHeight() - TOOLBAR_HEIGHT/2);
    
    analysisProgress->setSize(300, 30);
    if (directionView->isVisible())
        analysisProgress->setCentrePosition(getWidth()/2, getHeight() - TOOLBAR_HEIGHT - 30);
    else
        analysisProgress->setCentrePosition(getWidth()/2, getHeight() - TOOLBAR_HEIGHT - WAVEFORM_VIEW_HEIGHT - 30);
}


void MainComponent::timerCallback()
{
    if (waitingForFiles)
    {
        if (dataManager->isLoaded(loadingProgress))
        {
            waitingForFiles = false;
            waitingForAnalysis = true;
            
            loadingFilesProgress->setVisible(false);
            loadingProgress = 0.0;
            
            libraryView->loadFiles();
            
            libraryView->setVisible(true);
            libraryBtn->setVisible(true);
            directionBtn->setVisible(true);
            mixBtn->setVisible(true);
            volumeSld->setVisible(true);
            playPauseBtn->setVisible(true);
            skipBtn->setVisible(true);
            analysisProgress->setVisible(true);
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
            double sine = (std::sin(double(seconds) / 200) + 1.0) / 4.0;
            playPauseBtn->setAlpha(sine + 0.5f);
        }
    }
    
    if (dataManager->trackDataUpdate.load())
    {
        dataManager->trackDataUpdate.store(false);
        libraryView->updateData();
        directionView->updateData();
    }
    
    skipBtn->setEnabled(dj->canSkip());
}


void MainComponent::buttonClicked(juce::Button* button)
{
    int id = button->getComponentID().getIntValue();
    
    switch (id)
    {
        case ComponentIDs::chooseFolderBtn:
            chooseFolder();
            break;
            
        case ComponentIDs::libraryBtn:
            libraryView->setVisible(true);
            directionView->setVisible(false);
            mixView->setVisible(false);
            analysisProgress->setCentrePosition(getWidth()/2, getHeight() - TOOLBAR_HEIGHT - WAVEFORM_VIEW_HEIGHT - 30);
            break;
            
        case ComponentIDs::directionBtn:
            directionView->setVisible(true);
            libraryView->setVisible(false);
            mixView->setVisible(false);
            analysisProgress->setCentrePosition(getWidth()/2, getHeight() - TOOLBAR_HEIGHT - 30);
            break;
            
        case ComponentIDs::mixBtn:
            mixView->setVisible(true);
            directionView->setVisible(false);
            libraryView->setVisible(false);
            break;
            
        case ComponentIDs::playPauseBtn:
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
            
        case ComponentIDs::skipBtn:
            if (dj->canSkip())
                audioProcessor->skip();
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
        case ComponentIDs::volumeSld:
            audioProcessor->setVolume(slider->getValueObject().getValue());
            break;
            
        default:
            jassert(false); // Unrecognised button ID
    }
}


void MainComponent::chooseFolder()
{
    juce::FileChooser chooser ("Choose Music Folder");
    if (chooser.browseForDirectory())
    {
        dataManager->initialise(chooser.getResult(), directionView.get());
        
        waitingForFiles = true;
        
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
    
//    customAppearance.setColourScheme(juce::LookAndFeel_V4::getMidnightColourScheme());
    
    juce::LookAndFeel::setDefaultLookAndFeel(&customAppearance);
}

