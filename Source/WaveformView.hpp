//
//  WaveformView.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 06/05/2021.
//

#ifndef WaveformView_hpp
#define WaveformView_hpp

#include "WaveformLoader.hpp"

#define WAVEFORM_VIEW_HEIGHT (WAVEFORM_HEIGHT + WAVEFORM_BAR_HEIGHT)


class WaveformView : public juce::Component
{
public:
    
    WaveformView(bool scrollBarAtBottom);
    
    ~WaveformView() {}
    
    void resized() override;
    
    void load(Track* track);
    
    void update(int playhead, double timeStretch, double gain);
    
    void reset();
    
private:
    
    std::unique_ptr<WaveformComponent> waveform;
    std::unique_ptr<WaveformScrollBar> scrollBar;
    std::unique_ptr<WaveformLoader> loader;
    
    bool scrollBarBottom;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformView)
};


#endif /* WaveformView_hpp */
