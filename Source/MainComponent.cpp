#include "MainComponent.h"

//==============================================================================
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
    audioProcessor.reset(new AudioProcessor(dataManager.get()));
    
    library.reset(new LibraryComponent(audioProcessor.get(), dataManager.get()));
    addAndMakeVisible(library.get());
    
    dataManager->setLibrary(library.get());
    
#ifdef SHOW_GRAPH
    graphWindow.reset(new juce::ResizableWindow("Data Graph", true));
    graphWindow->setSize(600, 600);
    graphWindow->setUsingNativeTitleBar(true);
    graphWindow->setCentrePosition(400, 400);
    graphWindow->setVisible(true);
    graphWindow->setResizable(true, true);
    graphWindow->setContentOwned(new GraphComponent(), false);
#endif
    
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 100);
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
    library->setSize(getWidth(), getHeight());
    library->setTopLeftPosition(0, 0);
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
