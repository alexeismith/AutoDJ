//
//  GraphComponent.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 08/04/2021.
//

#include "GraphComponent.hpp"

// Number of y-axis pixels to reserve for the graph title and value read-out
#define GRAPH_TITLE_PADDING (20)


GraphComponent::GraphComponent()
{
    // Initialise the graph as not ready, because it hasn't received data yet
    ready.store(false);
    
    // Start the refresh timer, triggering a ~30fps repaint
    startTimerHz(30);
}


void GraphComponent::paint(juce::Graphics& g)
{
    // If the graph data isn't ready, return
    if (!ready.load()) return;
    
    // Set up variables to hold individual points in the graph, which will be connected by straight lines
    juce::Point<float> prevPoint;
    juce::Point<float> point;
    
    // Fill background with black
    g.setColour(juce::Colours::black);
    g.fillAll();
    
    // Draw title text in white font (draws nothing if title string is empty)
    g.setColour(juce::Colours::white);
    g.drawSingleLineText(title, getWidth()/2, 3*GRAPH_TITLE_PADDING/4, juce::Justification::horizontallyCentred);
    
    // Set the graph line colour
    g.setColour(juce::Colours::green);
    
    // Get the screen coord of the first data element
    point = getPositionForElement(0);
    
    // Iterate across all data elements
    for (int i = 1; i < dataStore.size(); i++)
    {
        // Store the current coord as the previous
        prevPoint = point;
        // Get the screen coord of the next data element
        point = getPositionForElement(i);
        
        // If the graph data is no longer valid, return
        if (!ready.load()) return;
        
        // Draw a straight line connecting the two points on the screen
        g.drawLine(juce::Line<float>(prevPoint, point));
    }
    
    // If the mouse is not hovering over a valid array element, return
    if (mouseOverElement < 0 || mouseOverElement >= dataStore.size())
        return;
    
    // Otherwise, proceed with highlighting the element that is hovered over...
    
    // Get the screen coord of the data element
    point = getPositionForElement(mouseOverElement);
    
    // Set the graphics colour
    g.setColour(juce::Colours::white);
    // Draw a circle at the position of the highlighted element
    g.fillEllipse(juce::Rectangle<float>(point.getX()-1,point.getY()-1,3,3));
    
    // Set the graphics colour
    g.setColour(juce::Colours::red);
    // Draw a text read-out of the array position and value of the data element
    g.drawSingleLineText(juce::String("x: " + juce::String(mouseOverElement) + ", y: " + juce::String(dataStore[mouseOverElement])), 10, 3*GRAPH_TITLE_PADDING/4);
}


void GraphComponent::timerCallback()
{
    // Simply trigger a repaint of the graph
    repaint();
}


void GraphComponent::mouseMove(const juce::MouseEvent& e)
{
    // Store the index of the data array element where the mouse is
    mouseOverElement = getElementAtPosition(e.getPosition().getX());
}


void GraphComponent::store(const GRAPH_DATA_TYPE* data, int size)
{
    // Set the thread-safe flag to indicate the graph data is not ready
    ready.store(false);
    
    // Clear the data array
    dataStore.clear();
    
    // Re-allocate the data array to fit the new data
    dataStore.resize(size);
    if (dataStore.size() != size) jassert(false); // Couldn't allocate!
    
    // Copy the supplied data into the graph storage
    memcpy(dataStore.getRawDataPointer(), data, size * sizeof(GRAPH_DATA_TYPE));
    
    // Find the range of the data in the array
    // and store it in the member variables minY and minX
    juce::findMinAndMax(dataStore.getRawDataPointer(), size, minY, maxY);
    
    // Set the thread-safe flag to indicate the graph data is now ready
    ready.store(true);
}


void GraphComponent::setTitle(juce::String str)
{
    // Set the thread-safe flag to indicate the graph data is not ready
    ready.store(false);
    
    // Copy the supplied title string into the graph member variable
    title = str;
    
    // Set the thread-safe flag to indicate the graph data is now ready
    ready.store(true);
}


juce::Point<float> GraphComponent::getPositionForElement(int index)
{
    // Find the horizontal position of the given data array element
    float x = getWidth() * (float(index) / dataStore.size());
    
    // Get the value of the given data element, as a float
    float value = float(dataStore[index]);
    
    // Find the range of y values
    float range = abs(maxY - minY);
    // Find the proportion of this range that the current value represents
    float proportion = value - minY;
    
    // Find the graph y-coord that represents the value
    float y = (getHeight()-GRAPH_TITLE_PADDING) * (proportion / range);
    // Reverse y to account for screen coordinate system
    y = getHeight() - y;
    
    // Return the x and y coords as a point
    return juce::Point<float>(x, y);
}


int GraphComponent::getElementAtPosition(float xPos)
{
    // Find the array index that corresponds to the supplied x-position
    return round((xPos / getWidth()) * dataStore.size());
}
