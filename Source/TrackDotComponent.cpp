//
//  TrackDotComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 09/05/2021.
//

#include "TrackDotComponent.hpp"

#define DOT_SIZE (7)
#define DOT_SIZE_HIGHLIGHT (14)


TrackDotComponent::TrackDotComponent(TrackInfo* track)
{
    info = track;
    
    setTooltip(info->getArtistTitle());

    colour = CamelotKey(info->key).getColour();
    
    dotSize = DOT_SIZE;
    
    setSize(DOT_SIZE_HIGHLIGHT, DOT_SIZE_HIGHLIGHT);
}


void TrackDotComponent::paint(juce::Graphics& g)
{
    g.setColour(colour);
    g.fillEllipse(getLocalBounds().withSizeKeepingCentre(dotSize, dotSize).toFloat());
}


void TrackDotComponent::timerCallback()
{
    if (isMouseOver())
        return;
    
    juce::uint32 seconds = juce::Time::getApproximateMillisecondCounter();
    double sine = (std::sin(double(seconds) / 100) + 1.0) / 2.0;
    
    dotSize = DOT_SIZE + sine * (DOT_SIZE_HIGHLIGHT - DOT_SIZE);
    repaint();
}


void TrackDotComponent::mouseEnter(const juce::MouseEvent& e)
{
    dotSize = DOT_SIZE_HIGHLIGHT;
    repaint();
}


void TrackDotComponent::mouseExit(const juce::MouseEvent& e)
{
    dotSize = DOT_SIZE;
    repaint();
}


void TrackDotComponent::update()
{
    if (info->playing)
    {
        startTimerHz(30);
    }
    else if (info->played)
    {
        stopTimer();
        dotSize = DOT_SIZE;
        colour = colour.withAlpha(0.25f);
    }
}


juce::Point<float> TrackDotComponent::getCentre()
{
    juce::Point<float> centre;
    centre.setX(getX() + getWidth() * 0.5f);
    centre.setY(getY() + getHeight() * 0.5f);
    
    return centre;
}


void TrackDotComponent::setPosition(float x, float y)
{
    xProportion = x;
    yProportion = y;
}
