//
//  WaveformScrollBar.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 04/05/2021.
//

#ifndef WaveformScrollBar_hpp
#define WaveformScrollBar_hpp

#include "WaveformComponent.hpp"

#define WAVEFORM_SCROLL_BAR_HEIGHT (30) ///< Height of WaveformScrollBar, in pixels


/**
 Extension of WaveformComponent which render the full track in a much thinner area.
 Includes a rectangular window indicating the current position in the track.
 For the window to be the correct width, this component must be the same width as its associated WaveformComponent.
 */
class WaveformScrollBar : public WaveformComponent
{
public:
    
    /** Constructor. */
    WaveformScrollBar();
    
    /** Destructor. */
    ~WaveformScrollBar() {}
    
    /** Called by the JUCE message when this component is resized - set size/position of child components here. */
    void resized() override;
    
    /** Called by the JUCE message thread to paint this component.
     
     @param[in] g  JUCE graphics handler */
    void paint(juce::Graphics& g) override;
    
    /** Updates the track playhead position, time-stretch and gain values.
    
    @param[in] playhead  Track playhead position (current audio sample reached in track)
    @param[in] timeStretch Stretch factor being applied to the track
    @param[in] gain Volume gain of the track (determines brightness of zoomed waveform) */
    void update(int playhead, double timeStretch, double gain = 1.0) override;
    
    /** Fetches the width of the track position window.
     
     @return Current width of window, in pixels */
    int getWindowWidth() { return windowWidth; }
    
private:
    
    /** Renders the full track waveform to an image.
    Playhead position, time stretch and gain are applied dynamically to this image before painting.
    This expensive function can therefore be called just once - upon track load.
    
    @param[in] colours Pointer to an array of colours, representing the colour to render in each waveform frame
    @param[in] levels Pointer to an array of levels, representing the magnitude/height of each waveform frame */
    void draw(juce::Array<juce::Colour>* colours, juce::Array<float>* levels) override;
    
    /** Checks whether a given waveform frame contains a beat in the track.
    
    @param[in] frameIndex Index of the waveform frame to check
    @param[out] downbeat Indicates whether the frame contains a downbeat */
    bool isBeat(int frameIndex, bool& downbeat) override;
    
    int windowStartX; ///< X-coordinate of the left side of the positional window
    int windowWidth = 0; ///< Width of the positional window, in pixels
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformScrollBar) ///< JUCE macro to add a memory leak detector
};

#endif /* WaveformScrollBar_hpp */
