//
//  WaveformComponent.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 08/04/2021.
//

#ifndef WaveformComponent_hpp
#define WaveformComponent_hpp

#include <JuceHeader.h>
#include "Track.hpp"


#define WAVEFORM_HEIGHT (100) ///< Height of WaveformComponent, in pixels

#define WAVEFORM_FRAME_SIZE (380) ///< Number of audio samples to consider for each waveform frame


/**
 Shows a coloured waveform representing audio data.
 Colour communicates timbre, where red, green and blue represent low, mid and high frequencies respectively.
 Combinations of these frequency bands can then produce the rest of the colour spectrum.
 */
class WaveformComponent : public juce::Component
{
public:
    
    /** Constructor. */
    WaveformComponent();
    
    /** Destructor. */
    ~WaveformComponent() {}

    /** Called by the JUCE message when this component is resized - set size/position of child components here. */
    void resized() override;
    
    /** Called by the JUCE message thread to paint this component.
     
     @param[in] g  JUCE graphics handler */
    virtual void paint(juce::Graphics& g) override;
    
    /** Updates the track playhead position, time-stretch and gain values.
     
     @param[in] playhead  Track playhead position (current audio sample reached in track)
     @param[in] timeStretch Stretch factor being applied to the track
     @param[in] gain Volume gain of the track (determines brightness of zoomed waveform) */
    virtual void update(int playhead, double timeStretch, double gain);
    
    /** Resets state, ready for a new track to be loaded. */
    void reset();
    
    /** Loads a new track and the associated data to display.
     Waveform data is provided in arrays which determine the colour and level of each waveform frame.
     A waveform frame is a single position along the horizontal axis which corresponds to a small window of audio data.
     The size of the window is determined by WAVEFORM_FRAME_SIZE.
     
     @param[in] track Pointer to the track information
     @param[in] colours Pointer to an array of colours, representing the colour to render in each waveform frame
     @param[in] levels Pointer to an array of levels, representing the magnitude/height of each waveform frame */
    void load(Track* track, juce::Array<juce::Colour>* colours, juce::Array<float>* levels);
    
    /** Inserts a red mix marker at the given position in the loaded track.
     
     @param[in] sample Position in track where marker should be placed (audio sample number, which will be converted to waveform frame number)
     @param[in] thickness Thickness of the marker to render */
    void insertMarker(int sample, int thickness = 3) { markers.add(sample); markerThickness = thickness; }
    
    /** Removes all mix markers. */
    void clearMarkers() { markers.clear(); }
    
protected:
    
    /** Renders the full track waveform to an image.
     Playhead position, time stretch and gain are applied dynamically to this image before painting.
     This expensive function can therefore be called just once - upon track load.
     
     @param[in] colours Pointer to an array of colours, representing the colour to render in each waveform frame
     @param[in] levels Pointer to an array of levels, representing the magnitude/height of each waveform frame */
    virtual void draw(juce::Array<juce::Colour>* colours, juce::Array<float>* levels);
    
    /** Draws the mix markers on top of the waveform.
     
     @param[in] g JUCE graphics handler, which is bound to the base waveform image. */
    virtual void drawMarkers(juce::Graphics& g);
    
    /** Checks whether a given waveform frame contains a beat in the track.
     
     @param[in] frameIndex Index of the waveform frame to check
     @param[out] downbeat Indicates whether the frame contains a downbeat */
    virtual bool isBeat(int frameIndex, bool& downbeat);
    
    /** Checks whether a given waveform frame contains a mix marker. */
    bool isMarker(int frameIndex);
    
    std::atomic<bool> ready = false; ///< Thread-safe flag to indicate whether the graph data is ready to display
    
    int numFrames = 0; ///< Number of waveform frames, determined by length of current track
    int startFrame = 0; ///< Index of the first frame to render at the left of the waveform
    
    int beatMarkerHeight; ///< Height of the white beat markers (rendered at top and bottom of waveform)
    
    juce::Image image; ///< Image of raw, unmodified track waveform, which is rendered in the draw() function
    juce::Image imageScaled; ///< Modified waveform, which takes into account playhead position, time-stretch, and volume gain
    
    juce::Colour colourBackground; ///< Base colour to paint as the background
    
private:
    
    Track* track; ///< Pointer to current track information
    
    juce::Array<int> markers; ///< Array of mix markers (stored in terms of audio sample, not waveform frame number)
    int markerThickness = 3; ///< Thickness to render mix markers, in pixels
    
    int imageOffset = 0; ///< Horizontal offset at which to render waveform image, in pixels (determined by playhead position in track)
    
    float brightness = 0.0; ///< Brightness at which to render waveform colours, determined by current volume gain of track (0.0 to 1.0)
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformComponent) ///< JUCE macro to add a memory leak detector
};

#endif /* WaveformComponent_hpp */
