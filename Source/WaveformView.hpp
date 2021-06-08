//
//  WaveformView.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 06/05/2021.
//

#ifndef WaveformView_hpp
#define WaveformView_hpp

#include "WaveformLoader.hpp"

#define WAVEFORM_VIEW_HEIGHT (WAVEFORM_HEIGHT + WAVEFORM_SCROLL_BAR_HEIGHT) ///< Height of WaveformView, in pixels (determined by height of both the waveforms it contains)


/**
 UI component which holds a large/zoomed track waveform, and a thin waveform showing the whole track.
 Handles loading of the waveform data on a dedicated thread.
 */
class WaveformView : public juce::Component
{
public:
    
    /** Constructor. */
    WaveformView(DataManager* dm, bool clickToScroll, bool scrollBarAtBottom, bool hideWhenEmpty);
    
    /** Destructor. */
    ~WaveformView() {}
    
    /** Called by the JUCE message when this component is resized - set size/position of child components here. */
    void resized() override;
    
    /** Triggers the loading of a new track into the waveforms, on a separate thread.
     
     @param[in] track Pointer to the track information
     @param[in] force If the supplied track is already loaded, this indicates whether it should be re-loaded */
    void load(Track* track, bool force = false);
    
    /** Updates the track playhead position, time-stretch and gain values.
     
     @param[in] playhead  Track playhead position (current audio sample reached in track)
     @param[in] timeStretch Stretch factor being applied to the track
     @param[in] gain Volume gain of the track (determines brightness of zoomed waveform) */
    void update(int playhead, double timeStretch = 1.0, double gain = 1.0);
    
    /** Triggers an update of playhead, timestretch and gain, then a repaint. */
    void refresh();
    
    /** Resets the state of each waveform, ready for a new track to be loaded. */
    void reset();
    
    /** Inserts a red mix marker into both waveforms.
     
     @param[in] sample Audio sample / position in track at which to insert the marker */
    void insertMarker(int sample);
    
    /** Removes all mix markers in both waveforms. */
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
    
    /** Scrolls the zoomed waveform to a new postion.
     
     @param[in] pos Position along the waveform scroll bar which should be shown at the centre of the zoomed waveform */
    void scroll(juce::Point<int> pos);
    
    std::unique_ptr<WaveformComponent> waveform; ///< Large/zoomed waveform
    std::unique_ptr<WaveformScrollBar> scrollBar; ///< Thin scroll bar waveform
    
    bool scrollBarBottom; ///< Indicates whether the thin waveform is shown below, or above, the zoomed waveform
    
    std::unique_ptr<WaveformLoader> loader; ///< Thread for loading waveform data
    
    bool scrollable; ///< Indicates whether the thin waveform can be used to scroll along the track (true in Library view, not true in Mix view)
    bool scrolling = false; ///< Flag to track whether a scroll is in progress
    
    int playhead; ///< Track playhead position (current audio sample reached in track)
    int trackLength; ///< Length of the track in samples
    
    double timeStretch; ///< Current time-stretch factor applied to the track
    double gain; ///< Current volume gain applied to the track
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformView) ///< JUCE macro to add a memory leak detector
};


#endif /* WaveformView_hpp */
