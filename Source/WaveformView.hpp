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
    
    WaveformView(DataManager* dm, bool clickToScroll, bool scrollBarAtBottom, bool hideWhenEmpty);
    
    ~WaveformView() {}
    
    void resized() override;
    
    void load(Track* track, bool force = false);
    
    void update(int playhead, double timeStretch = 1.0, double gain = 1.0);
    
    void refresh();
    
    void reset();
    
    void insertMarker(int sample);
    
    void clearMarkers();
    
    void mouseDown(const juce::MouseEvent &event) override { scroll(event.getPosition()); }
    void mouseUp(const juce::MouseEvent &event) override { scrolling = false; }
    void mouseDrag(const juce::MouseEvent &event) override { scroll(event.getPosition()); }
    
private:
    
    void scroll(juce::Point<int> pos);
    
    std::unique_ptr<WaveformComponent> waveform;
    std::unique_ptr<WaveformScrollBar> scrollBar;
    std::unique_ptr<WaveformLoader> loader;
    
    bool scrollable, scrolling = false;
    bool scrollBarBottom;
    
    int playhead, trackLength;
    double timeStretch, gain;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformView)
};


#endif /* WaveformView_hpp */
