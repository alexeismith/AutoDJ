//
//  DirectionLine.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 12/05/2021.
//

#include "DirectionLine.hpp"

#define LINE_THICKNESS (3)


void DirectionLine::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::white);
    g.drawDashedLine(line, dashLength, 2, LINE_THICKNESS);
}


void DirectionLine::update(TrackDotComponent* leader, TrackDotComponent* follower)
{
    if (follower == nullptr)
    {
        juce::MessageManager::callAsync(std::function<void()>([this]() {
            setVisible(false);
        }));
    
        return;
    }
    
    juce::uint32 counter = juce::Time::getApproximateMillisecondCounter() / 50;
    counter %= 10;

    juce::Path path;
    path.startNewSubPath(leader->getCentre());
    path.lineTo(follower->getCentre());
    
    bounds = path.getBounds();
    bounds.expand(LINE_THICKNESS, LINE_THICKNESS);
    
    juce::Point<float> start = path.getPointAlongPath(counter) - bounds.getPosition();
    juce::Point<float> end = follower->getCentre() - bounds.getPosition();
    
    line = juce::Line<float>(start, end);

    juce::MessageManager::callAsync(std::function<void()>([this]() {
        updateBounds();
        setVisible(true);
        repaint();
    }));
}


void DirectionLine::updateBounds()
{
    setBounds(bounds.toNearestInt());
}
