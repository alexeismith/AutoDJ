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


void TrackDotComponent::mouseEnter(const juce::MouseEvent &event)
{
    dotSize = DOT_SIZE_HIGHLIGHT;
    repaint();
}


void TrackDotComponent::mouseExit(const juce::MouseEvent &event)
{
    dotSize = DOT_SIZE;
    repaint();
}


void TrackDotComponent::updateColour()
{
    if (info->played)
        colour = colour.withAlpha(0.2f);
}


void TrackDotComponent::updatePosition(float x, float y)
{
    xProportion = x;
    yProportion = y;
}
