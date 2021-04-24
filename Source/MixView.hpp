//
//  MixView.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/04/2021.
//

#ifndef MixView_hpp
#define MixView_hpp

#include <JuceHeader.h>

#include "TrackDataManager.hpp"
#include "WaveformComponent.hpp"

class MixView : public juce::Component, public juce::Button::Listener
{
public:
    
    MixView(TrackDataManager* dm);
    
    ~MixView() {}
    
    void resized() override;
    
    void buttonClicked(juce::Button* button) override;
    
private:
    
    TrackDataManager* dataManager;
    
    std::unique_ptr<WaveformComponent> waveform;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MixView)
};

#endif /* MixView_hpp */
