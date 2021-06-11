//
//  WaveformComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 08/04/2021.
//

#include "WaveformComponent.hpp"

#include "CommonDefs.hpp"

#define WAVEFORM_BEAT_MARKER_SIZE (0.1f)


WaveformComponent::WaveformComponent()
{
    colourBackground = getLookAndFeel().findColour(juce::ListBox::backgroundColourId).withBrightness(0.08f);
    
    setBufferedToImage(true); // Optimises JUCE rendering
    setOpaque(true); // Optimises JUCE rendering
    
    setSize(0, WAVEFORM_HEIGHT);
    
    setInterceptsMouseClicks(false, false);
}


void WaveformComponent::resized()
{
    beatMarkerHeight = WAVEFORM_BEAT_MARKER_SIZE * getHeight();
}


void WaveformComponent::paint(juce::Graphics& g)
{
    // Fill background colour
    g.setColour(colourBackground);
    g.fillAll();
    
    // If the waveform is not loaded, return
    if (!ready.load()) return;

    // Simply draw the pre-prepared waveform image
    // This is draw at a horizontal offset if the image does not take up the full waveform width
    // (i.e. if we're at the start of the track)
    g.drawImageAt(imageScaled, imageOffset, 0);
    
    // Draw the background colour on top of the image, to give it a brightness
    // (Think this is more efficient than applying alpha to the image)
    g.setColour(colourBackground.withAlpha(1.f - brightness));
    g.fillAll();
}


void WaveformComponent::update(int playhead, double timeStretch, double gain)
{
    if (!ready.load()) return;
    
    // Convert gain to brightess
    // Take square root to give it a logarithmic-like response (so its less dark for mid-gain values)
    brightness = float(std::sqrt(gain));
    // Limit the minimum brightness to 0.3, so we can see the waveform all the time
    brightness = juce::jmax(brightness, 0.3f);
    
    // Get the playhead in terms of waveform frames
    playhead = round(float(playhead) / WAVEFORM_FRAME_SIZE);
    
    // Find the visible number of pixels in the waveform, based on the time-stretch that will be applied
    // Each waveform pixel corresponds to a frame of audio (WAVEFORM_FRAME_SIZE in length)
    int visibleWidth = getWidth() * timeStretch;
    
    // We want the playhead positioned in the middle of the screen,
    // so the left-most frame is: the playhead minus half the visible width
    startFrame = playhead - visibleWidth / 2;
    
    // Now we manipulate the full track waveform that was rendered to 'image',
    // to produce a version that takes into account playhead position and time stretch: 'imageScaled'
    
    // Copy the relevant portion of the original image
    imageScaled = image.getClippedImage(juce::Rectangle<int>(startFrame, 0, visibleWidth, getHeight()));
    
    // If we are at the start of end of the track, imageScaled won't take up the full width of the waveform
    // So find how many missing pixels there are along the horizontal axis (if any)
    int missingWidth = round(double(visibleWidth - imageScaled.getWidth()) / timeStretch);
    
    // If there is missing width, and the playhead is in the first half of the waveform, we are at the track start
    if (missingWidth > 0 && playhead < numFrames / 2)
        // Add a horizontal offset to move the start to the centre
        imageOffset = missingWidth;
    else // Otherwise, the playhead is already centred
        imageOffset = 0;
    
    // Stretch the image horizontally, using JUCE's medium resampling quality
    // to compromise between efficiency and fidelity (low resampling gave too much pixelation)
    imageScaled = imageScaled.rescaled(getWidth() - missingWidth, getHeight(), juce::Graphics::ResamplingQuality::mediumResamplingQuality);
    
    // Trigger a re-paint by invalidating the component's image buffer
    getCachedComponentImage()->invalidateAll();
}


void WaveformComponent::load(Track* t, juce::Array<juce::Colour>* colours, juce::Array<float>* levels)
{
    track = t;
    numFrames = levels->size();
    draw(colours, levels);
    
    ready.store(true);
    
    update(0, 1.0, 1.0);
}


void WaveformComponent::draw(juce::Array<juce::Colour>* colours, juce::Array<float>* levels)
{
    float magnitude;
    
    // Create a new image to draw to, which will be manipulated later to
    // react to playhead position and time stretch
    image = juce::Image(juce::Image::RGB, numFrames, getHeight(), true);
    // Create a JUCE graphics handler to paint on the image
    juce::Graphics g(image);
    
    // Fill the background with colour
    // This MUST be done because image is marked as opaque, so pixels underneath are undefined
    g.setColour(colourBackground);
    g.fillAll();
    
    // For each waveform frame
    for (int frame = 0; frame < numFrames; frame++)
    {
        // Fetch the magnitude of the wave
        magnitude = levels->getReference(frame) * getHeight() * 0.4f;
        
        // Fetch the colour of the frame
        g.setColour(colours->getReference(frame));
        
        // Draw a vertical line
        g.drawVerticalLine(frame, 0.5f * getHeight() - magnitude, 0.5f * getHeight() + magnitude);
    }
    
    // Draw any beat markers on top
    drawMarkers(g);
}


void WaveformComponent::drawMarkers(juce::Graphics& g)
{
    bool downbeat;
    
    // Markers will be white
    g.setColour(juce::Colours::white);
    
    // For each waveform frame
    for (int frame = 0; frame < numFrames; frame++)
    {
        // If a beat is within the frame
        if (isBeat(frame, downbeat))
        {
            // If the beat is a downbeat, draw a full-height line
            if (downbeat)
            {
                g.drawLine(frame, 0, frame, getHeight(), 2);
            }
            else // Otherwise, draw small lines top and bottom
            {
                g.drawLine(frame, 0, frame, beatMarkerHeight, 2);
                g.drawLine(frame, getHeight()-beatMarkerHeight, frame, getHeight(), 2);
            }
        }
    }
    
    // Draw any mix markers in red...
    
    g.setColour(juce::Colours::red);
    
    for (int frame = 0; frame < numFrames; frame++)
    {
        if (isMarker(frame))
        {
            g.drawLine(frame, 0, frame, getHeight(), markerThickness);
        }
    }
}


void WaveformComponent::reset()
{
    ready.store(false);
    
    getCachedComponentImage()->invalidateAll();
    
    numFrames = startFrame = imageOffset = 0;
}


bool WaveformComponent::isBeat(int frameIndex, bool& downbeat)
{
    int remainder, frameStart, frameEnd;
    
    downbeat = false;
    
    if (track->info->bpm == -1) return false;
    
    frameStart = frameIndex * WAVEFORM_FRAME_SIZE;
    frameEnd = frameStart + WAVEFORM_FRAME_SIZE - 1;
    
    double beatLength = AutoDJ::getBeatPeriod(track->info->bpm);
    
    frameStart -= track->info->beatPhase;
    frameEnd -= track->info->beatPhase;
    
    if (abs(floor(frameEnd/beatLength) - floor(frameStart/beatLength)) > 0)
    {
        remainder = (int(floor(frameEnd/beatLength)) - track->info->downbeat) % BEATS_PER_BAR;
        
        if (remainder == 0 || remainder == BEATS_PER_BAR)
            downbeat = true;
        
        return true;
    }
    
    return false;
}


bool WaveformComponent::isMarker(int frameIndex)
{
    int frameStart = frameIndex * WAVEFORM_FRAME_SIZE;
    int frameEnd = frameStart + WAVEFORM_FRAME_SIZE - 1;
    
    // We could use a more efficient search method, but there shouldn't
    // ever be more than a few markers, so this is fine...
    
    for (int marker : markers)
    {
        if (marker >= frameStart && marker <= frameEnd)
            return true;
    }
    
    return false;
}
