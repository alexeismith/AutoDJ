//
//  LibraryComponent.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/02/2021.
//

#ifndef LibraryComponent_hpp
#define LibraryComponent_hpp

#include "TrackDataManager.hpp"
#include "QueueTableComponent.hpp"
#include "CommonDefs.hpp"

// TODO: temp
#include "WaveformBarComponent.hpp"

class LibraryView : public juce::Component, public juce::Button::Listener, public juce::Timer
{
public:
    
    LibraryView(TrackDataManager* dm, juce::Button* play);
    
    ~LibraryView() {}
    
    void resized() override;
    
    void buttonClicked(juce::Button* button) override;
    
    void timerCallback() override;
    
    void loadFiles();
    
private:
    
    void chooseFolder();
    
    std::unique_ptr<TrackTableComponent> trackTable;
    
    juce::Button* playBtn;
    std::unique_ptr<juce::Button> chooseFolderBtn;
    std::unique_ptr<juce::ProgressBar> loadingFilesProgress;
    std::unique_ptr<juce::ProgressBar> analysisProgress;
    
    TrackDataManager* dataManager;
    
    // TODO: temp
    std::unique_ptr<WaveformComponent> waveform;
    std::unique_ptr<WaveformBarComponent> waveformBar;
    std::unique_ptr<WaveformLoadThread> waveformLoader;
    TrackInfo info;
    Track track;
    
    bool waitingForFiles = false;
    bool waitingForAnalysis = false;
    
    double loadingProgress = 0.0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LibraryView)
};

#endif /* LibraryComponent_hpp */
