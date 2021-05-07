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
    ready.store(false);
    
    setBufferedToImage(true);
    setOpaque(true);
    
    setSize(0, WAVEFORM_HEIGHT);
    
    setInterceptsMouseClicks(false, false);
}


void WaveformComponent::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::black);
    g.fillAll();
    
    if (!ready.load()) return;

    g.drawImageAt(imageScaled, imageOffset, 0);
    
    g.setColour(juce::Colours::black.withAlpha(1.f - brightness));
    g.fillAll();
}


void WaveformComponent::resized()
{
    beatMarkerHeight = WAVEFORM_BEAT_MARKER_SIZE * getHeight();
}


void WaveformComponent::update(int playhead, double timeStretch, double gain)
{
    if (!ready.load()) return;
    
    brightness = juce::jmax(float(std::sqrt(gain)), 0.3f);
    
    int visibleWidth = getWidth() * timeStretch;
    
    playhead -= double(WAVEFORM_FRAME_SIZE * visibleWidth) / 2;
    
    startFrame = round(double(playhead) / WAVEFORM_FRAME_SIZE);
    
    imageScaled = image.getClippedImage(juce::Rectangle<int>(startFrame, 0, visibleWidth, getHeight()));
    
    imageOffset = round(double(visibleWidth - imageScaled.getWidth()) / timeStretch);
    
    imageScaled = imageScaled.rescaled(getWidth() - imageOffset, getHeight(), juce::Graphics::ResamplingQuality::mediumResamplingQuality);
    
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
    image = juce::Image(juce::Image::RGB, numFrames, getHeight(), true);
    juce::Graphics g(image);
    
    float magnitude;
    bool downbeat;
    
    g.setColour(juce::Colours::black);
    g.fillAll();
    
    for (int frame = 0; frame < numFrames; frame++)
    {
        if (frame > 0 && frame < numFrames)
        {
            magnitude = levels->getReference(frame) * getHeight() * 0.4f;
            
            g.setColour(colours->getReference(frame));
            g.drawVerticalLine(frame, 0.5f * getHeight() - magnitude, 0.5f * getHeight() + magnitude);
        }
        else
        {
            g.setColour(juce::Colours::darkgrey);
            g.drawVerticalLine(frame, 0, getHeight());
        }
        
        if (isBeat(frame, downbeat))
        {
            g.setColour(juce::Colours::white);
            
            if (downbeat)
            {
                g.drawVerticalLine(frame, 0, getHeight());
                g.drawVerticalLine(frame+1, 0, getHeight());
            }
            else
            {
                g.drawVerticalLine(frame, 0, beatMarkerHeight);
                g.drawVerticalLine(frame, getHeight() - beatMarkerHeight, getHeight());
                g.drawVerticalLine(frame+1, 0, beatMarkerHeight);
                g.drawVerticalLine(frame+1, getHeight() - beatMarkerHeight, getHeight());
            }
        }
    }
    
    drawMarkers(g);
}


void WaveformComponent::drawMarkers(juce::Graphics& g)
{
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
    
    numFrames = startFrame = 0;
}


bool WaveformComponent::isBeat(int frameIndex, bool& downbeat)
{
    int remainder, frameStart, frameEnd;
    
    downbeat = false;
    
    if (track->info->bpm == -1) return false;
    
    frameStart = frameIndex * WAVEFORM_FRAME_SIZE;
    frameEnd = frameStart + WAVEFORM_FRAME_SIZE - 1;
    
    double beatLength = 60 * SUPPORTED_SAMPLERATE / track->info->bpm;
    
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
    
    for (int marker : markers)
    {
        if (marker >= frameStart && marker <= frameEnd)
            return true;
    }
    
    return false;
}
