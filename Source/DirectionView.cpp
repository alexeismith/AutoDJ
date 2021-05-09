//
//  DirectionView.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 09/05/2021.
//

#include "DirectionView.hpp"

#define SIDE_PADDING (50)


DirectionView::DirectionView(AnalysisManager* am) :
    analysisManager(am)
{
    toolTip.reset(new juce::TooltipWindow(this, 200));
    addAndMakeVisible(toolTip.get());
}


void DirectionView::paint(juce::Graphics& g)
{
    g.setGradientFill(juce::ColourGradient(juce::Colours::darkgrey, getWidth()/2, getHeight()/4, juce::Colours::darkgrey.darker(), getWidth(), getHeight(), true));
    g.fillAll();
}


void DirectionView::resized()
{
    int x, y;
    int widthAvailable = getWidth() - 2 * SIDE_PADDING;
    int heightAvailable = getHeight() - 2 * SIDE_PADDING;
    
    for (auto* dot : dots)
    {
        x = (dot->getXProportion() * widthAvailable) + SIDE_PADDING;
        y = (dot->getYProportion() * heightAvailable) + SIDE_PADDING;
        dot->setCentrePosition(x, y);
    }
}


void DirectionView::updateData()
{
    for (auto* dot : dots)
    {
        dot->updateColour();
    }
    
    repaint();
}


void DirectionView::addAnalysed(TrackInfo* track)
{
    dots.add(new TrackDotComponent(track));
    calculatePositions();
    
    juce::MessageManager::callAsync(std::function<void()>([this]() {
        this->addAndMakeVisible(dots.getLast());
        this->resized();
    }));
}


void DirectionView::calculatePositions()
{
    AnalysisResults results = analysisManager->getResults();
    TrackInfo info;
    float xProportion, yProportion;
    
    for (auto* dot : dots)
    {
        info = *dot->getTrack();
        
        xProportion = float(info.bpm - results.minBpm) / (results.maxBpm - results.minBpm);
        yProportion = float(info.groove - results.minGroove) / (results.maxGroove - results.minGroove);
        
        // Flip y axis so high groove is at top
        yProportion = 1 - yProportion;
        
        dot->updatePosition(xProportion, yProportion);
    }
}
