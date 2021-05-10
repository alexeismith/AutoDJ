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
    toolTip.reset(new juce::TooltipWindow(this, 0));
    addAndMakeVisible(toolTip.get());
    
    colourBackground = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
}


void DirectionView::paint(juce::Graphics& g)
{
    g.setGradientFill(juce::ColourGradient(colourBackground.withBrightness(0.3f), getWidth()/2, getHeight()/4, colourBackground, getWidth()/2, getHeight(), true));
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
    for (int i = 0; i < dots.size(); i++)
    {
        dots.getUnchecked(i)->update();
        
        if (i >= numDotsAdded)
        {
            addAndMakeVisible(dots.getUnchecked(i));
            numDotsAdded += 1;
        }
    }
    
    repaint();
}


void DirectionView::addAnalysed(TrackInfo* track)
{
    const juce::ScopedLock sl(lock);
    
    dots.add(new TrackDotComponent(track));
    calculatePositions();
    
    juce::MessageManager::callAsync(std::function<void()>([this]() {
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
        
        dot->setPosition(xProportion, yProportion);
    }
}
