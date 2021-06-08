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

#define WAVEFORM_HEIGHT (100)

#define WAVEFORM_FRAME_SIZE (380)

// NOTE: need a separate instance for each audio channel to be visualised, because the IIR filters depend on previous samples
class WaveformComponent : public juce::Component
{
public:
    
    /** Constructor. */
    WaveformComponent();
    
    /** Destructor. */
    ~WaveformComponent() {}
    
    /** Called by the JUCE message thread to paint this component.
     
     @param[in] g  JUCE graphics handler */
    virtual void paint(juce::Graphics& g) override;
    
    void resized() override;
    
    virtual void update(int playhead, double timeStretch, double gain);
    
    void reset();
    
    void load(Track* t, juce::Array<juce::Colour>* colours, juce::Array<float>* levels);
    
    void insertMarker(int sample, int thickness = 3) { markers.add(sample); markerThickness = thickness; }
    
    void clearMarkers() { markers.clear(); }
    
protected:
    
    virtual void draw(juce::Array<juce::Colour>* colours, juce::Array<float>* levels);
    
    virtual void drawMarkers(juce::Graphics& g);
    
    virtual bool isBeat(int frameIndex, bool& downbeat);
    
    bool isMarker(int frameIndex);
    
    std::atomic<bool> ready;
    
    int numFrames = 0, startFrame = 0;
    
    int beatMarkerHeight;
    
    juce::Image image, imageScaled;
    
    juce::Colour colourBackground;
    
private:
    
    Track* track;
    
    juce::Array<int> markers;
    int markerThickness = 3;
    
    int imageOffset;
    
    float brightness = 0.0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformComponent) ///< JUCE macro to add a memory leak detector
};

#endif /* WaveformComponent_hpp */
