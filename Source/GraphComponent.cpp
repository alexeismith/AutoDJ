//
//  GraphComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 08/04/2021.
//

#include "GraphComponent.hpp"


GraphComponent::GraphComponent()
{
    ready.store(false);
    
    startTimerHz(30);
}


void GraphComponent::paint(juce::Graphics& g)
{
    if (!ready.load()) return;
    
    juce::Point<float> prevPoint;
    juce::Point<float> point;
    
    g.setColour(juce::Colours::black);
    g.fillAll();
    
    g.setColour(juce::Colours::white);
    
    point = getPositionForValue(0);
    
    for (int i = 1; i < dataStore.size(); i++)
    {
        prevPoint = point;
        point = getPositionForValue(i);
        
        g.drawLine(juce::Line<float>(prevPoint, point));
    }
    
    point = getPositionForValue(xVal);
    prevPoint.addXY(5, 5);
    
    g.setColour(juce::Colours::red);
    g.fillEllipse(juce::Rectangle<float>(point.getX()-1,point.getY()-1,3,3));
    
    g.setColour(juce::Colours::green);
    g.drawSingleLineText(juce::String(juce::String(xVal) + " " + juce::String(yVal)), 10, 20);
}


void GraphComponent::timerCallback()
{
    repaint();
}


void GraphComponent::mouseMove(const juce::MouseEvent& e)
{
    getValueForPosition(e.getPosition().toFloat());
}


void GraphComponent::store(GRAPH_DATA_TYPE* data, int size)
{
    ready.store(false);
    
    dataStore.clear();
    
    dataStore.resize(size);
    if (dataStore.size() != size) jassert(false); // Couldn't allocate!
    
    memcpy(dataStore.getRawDataPointer(), data, size);
    
    juce::findMinAndMax(dataStore.getRawDataPointer(), size, minY, maxY);
    
    ready.store(true);
}


void GraphComponent::store(GRAPH_DATA_TYPE* data, int size, GRAPH_DATA_TYPE xMin, GRAPH_DATA_TYPE xMax)
{
    ready.store(false);
    
    minX = xMin;
    maxX = xMax;
    
    store(data, size);
}


juce::Point<float> GraphComponent::getPositionForValue(int index)
{
    float x = getWidth() * (float(index) / dataStore.size());
    float y = getHeight() * (float(dataStore[index]) / (maxY - minY));
    
    // Reverse y to account for screen coordinate system
    y = getHeight() - y;
    
    return juce::Point<float>(x, y);
}


void GraphComponent::getValueForPosition(juce::Point<float> pos)
{
    xVal = round((pos.getX() / getWidth()) * dataStore.size());
    yVal = dataStore[xVal];
}
