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
    
    dataManager.reset(new TrackDataManager());
    dj.reset(new ArtificialDJ(dataManager.get()));
    
    audioProcessor.reset(new AudioProcessor(dataManager.get(), dj.get(), initBlockSize.load()));
    dj->setAudioProcessor(audioProcessor.get());
    
    libraryView.reset(new LibraryView(dataManager.get(), &loadingProgress));
    addChildComponent(libraryView.get());
    
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
    
    mixBtn.reset(new juce::TextButton("Mix"));
    mixBtn->setComponentID(juce::String(ComponentIDs::mixBtn));
    addChildComponent(mixBtn.get());
    mixBtn->addListener(this);
    
    playPauseBtn.reset(new juce::TextButton(">"));
    addChildComponent(playPauseBtn.get());
    playPauseBtn->setComponentID(juce::String(ComponentIDs::playPauseBtn));
    playPauseBtn->addListener(this);
    playPauseBtn->setEnabled(false);
    
    volumeSld.reset(new juce::Slider());
    addChildComponent(volumeSld.get());
    volumeSld->setComponentID(juce::String(ComponentIDs::volumeSld));
    volumeSld->addListener(this);
    volumeSld->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    volumeSld->setRange(0.f, 1.f);
    volumeSld->setSkewFactor(0.7);
    volumeSld->setValue(1.f);
    
    
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
    
    startTimerHz(20);
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
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    sizeLimits.checkComponentBounds(this);
    
    chooseFolderBtn->setSize(120, 40);
    chooseFolderBtn->setCentrePosition(getWidth()/2, getHeight()/2);
    
    loadingFilesProgress->setSize(150, 30);
    loadingFilesProgress->setCentrePosition(getWidth()/2, getHeight()/2);
    
    libraryView->setSize(getWidth(), getHeight() - TOOLBAR_HEIGHT);
    libraryView->setTopLeftPosition(0, 0);
    
    mixView->setSize(getWidth(), getHeight() - TOOLBAR_HEIGHT);
    mixView->setTopLeftPosition(0, 0);
    
    libraryBtn->setSize(80, 30);
    libraryBtn->setCentrePosition(libraryBtn->getWidth()/2 + 10, getHeight() - TOOLBAR_HEIGHT/2);
    
    mixBtn->setSize(50, 30);
    mixBtn->setCentrePosition(mixBtn->getWidth()/2 + 100, getHeight() - TOOLBAR_HEIGHT/2);
    
    playPauseBtn->setSize(28, 28);
    playPauseBtn->setCentrePosition(getWidth()/2, getHeight() - TOOLBAR_HEIGHT/2);
    
    volumeSld->setSize(140, 50);
    volumeSld->setCentrePosition(getWidth() - 80, getHeight() - TOOLBAR_HEIGHT/2);
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
            mixBtn->setVisible(true);
            volumeSld->setVisible(true);
            playPauseBtn->setVisible(true);
        }
    }
    
    if (waitingForAnalysis)
    {
        bool canStartPlaying;
        
        if (dataManager->analysisProgress(loadingProgress, canStartPlaying))
        {
            waitingForAnalysis = false;
            libraryView->analysisComplete();
        }
        
        if (canStartPlaying)
            playPauseBtn->setEnabled(true);
    }
    
    if (waitingForDJ)
    {
        if (dj->isInitialised())
        {
            waitingForDJ = false;
            playPauseBtn->setEnabled(true);
        }
    }
}


void MainComponent::setAppearance()
{
//    juce::LookAndFeel_V4::ColourScheme colourScheme = juce::LookAndFeel_V4::ColourScheme(juce::Colours::white,
//                                        juce::Colours::grey,
//                                        juce::Colours::lightgrey,
//                                        juce::Colours::black,
//                                        juce::Colours::black,
//                                        juce::Colours::grey,
//                                        juce::Colours::red,
//                                        juce::Colours::darkred,
//                                        juce::Colours::white);
//    
//    customAppearance.setColourScheme(colourScheme);
    
    juce::LookAndFeel::setDefaultLookAndFeel(&customAppearance);
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
            mixView->setVisible(false);
            break;
            
        case ComponentIDs::mixBtn:
            mixView->setVisible(true);
            libraryView->setVisible(false);
            break;
            
        case ComponentIDs::playPauseBtn:
            if (!dj->playPause())
            {
                playPauseBtn->setEnabled(false);
                waitingForDJ = true;
            }
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
        dataManager->initialise(chooser.getResult());
        
        waitingForFiles = true;
        
        chooseFolderBtn->setVisible(false);
        loadingFilesProgress->setVisible(true);
    }
}

