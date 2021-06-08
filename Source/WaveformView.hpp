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
    
    /** Constructor. */
    WaveformView(DataManager* dm, bool clickToScroll, bool scrollBarAtBottom, bool hideWhenEmpty);
    
    /** Destructor. */
    ~WaveformView() {}
    
    /** Called by the JUCE message when this component is resized - set size/position of child components here. */
    void resized() override;
    
    void load(Track* track, bool force = false);
    
    void update(int playhead, double timeStretch = 1.0, double gain = 1.0);
    
    void refresh();
    
    void reset();
    
    void insertMarker(int sample);
    
    void clearMarkers();
    
    /** Mouse input handler, called when the mouse is pressed on this component.
     
    @param[in] e Information about the input event that occured, such as mouse position */
    void mouseDown(const juce::MouseEvent& e) override { scroll(e.getPosition()); }
    
    /** Mouse input handler, called when the mouse is released after pressing on this component.
     
    @param[in] e Information about the input event that occured, such as mouse position */
    void mouseUp(const juce::MouseEvent& e) override { scrolling = false; }
    
    /** Mouse input handler, called when the mouse is dragged after pressing on this component.
     
    @param[in] e Information about the input event that occured, such as mouse position */
    void mouseDrag(const juce::MouseEvent& e) override { scroll(e.getPosition()); }
    
private:
    
    void scroll(juce::Point<int> pos);
    
    std::unique_ptr<WaveformComponent> waveform;
    std::unique_ptr<WaveformScrollBar> scrollBar;
    std::unique_ptr<WaveformLoader> loader;
    
    bool scrollable, scrolling = false;
    bool scrollBarBottom;
    
    int playhead, trackLength;
    double timeStretch, gain;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformView) ///< JUCE macro to add a memory leak detector
};


#endif /* WaveformView_hpp */
