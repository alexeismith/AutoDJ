//
//  TrackDotComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 09/05/2021.
//

#include "TrackDotComponent.hpp"

#define DOT_DIAMETER (7)
#define DOT_DIAMETER_HIGHLIGHTED (14)


TrackDotComponent::TrackDotComponent(TrackInfo* track)
{
    info = track;
    
    setTooltip(info->getArtistTitle());

    colour = CamelotKey(info->key).getColour();
    
    diameter = DOT_DIAMETER;
    
    setSize(DOT_DIAMETER_HIGHLIGHTED, DOT_DIAMETER_HIGHLIGHTED);
}


void TrackDotComponent::paint(juce::Graphics& g)
{
    g.setColour(colour);
    g.fillEllipse(getLocalBounds().withSizeKeepingCentre(diameter, diameter).toFloat());
}


void TrackDotComponent::timerCallback()
{
    if (isMouseOver())
        return;
    
    juce::uint32 seconds = juce::Time::getApproximateMillisecondCounter();
    double sine = (std::sin(double(seconds) / 100) + 1.0) / 2.0;
    
    diameter = DOT_DIAMETER + sine * (DOT_DIAMETER_HIGHLIGHTED - DOT_DIAMETER);
    repaint();
}


void TrackDotComponent::mouseEnter(const juce::MouseEvent& e)
{
    diameter = DOT_DIAMETER_HIGHLIGHTED;
    repaint();
}


void TrackDotComponent::mouseExit(const juce::MouseEvent& e)
{
    diameter = DOT_DIAMETER;
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
        diameter = DOT_DIAMETER;
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
