//
//  DirectionView.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 09/05/2021.
//

#include "DirectionView.hpp"

#define SIDE_PADDING (60)


DirectionView::DirectionView(AnalysisManager* am) :
    analysisManager(am)
{
    toolTip.reset(new juce::TooltipWindow(this, 0));
    addAndMakeVisible(toolTip.get());
    
    addChildComponent(directionLine);
    
    colourBackground = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
    
    logo = juce::ImageFileFormat::loadFrom(BinaryData::logoLarge_png, BinaryData::logoLarge_pngSize);
    axes = juce::ImageFileFormat::loadFrom(BinaryData::axes_png, BinaryData::axes_pngSize);
    
    setBufferedToImage(true);
    
    startTimer(30); // 33ms = 30.3Hz
}


void DirectionView::hiResTimerCallback()
{
    directionLine.update(leader, follower);
}


void DirectionView::paint(juce::Graphics& g)
{
    g.setGradientFill(juce::ColourGradient(colourBackground.withBrightness(0.3f), getWidth()/2, getHeight()/4, colourBackground, getWidth()/2, getHeight(), true));
    g.fillAll();
    
    g.drawImage(logo, logoArea, juce::RectanglePlacement::centred);
    g.drawImage(axes, axesArea, juce::RectanglePlacement::xLeft | juce::RectanglePlacement::yBottom);
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
    
    logoArea.setSize(getWidth()/2, getHeight()/2);
    logoArea.setCentre(getWidth()/2, getHeight()/2);
    
    axesArea.setSize(getHeight()/3, getHeight()/3);
    axesArea.setX(35);
    axesArea.setY(getHeight() - 25 - axesArea.getHeight());
    
    directionLine.update(leader, follower);
    directionLine.updateBounds();
}


void DirectionView::updateData()
{
    TrackDotComponent* dot;
    
    for (int i = 0; i < dots.size(); i++)
    {
        dot = dots.getUnchecked(i);
        
        dot->update();
        
        if (i >= numDotsAdded)
        {
            addAndMakeVisible(dots.getUnchecked(i));
            numDotsAdded += 1;
        }
        
        
        if (leader != nullptr)
        {
            if (dot->getTrack()->hash == leader->getTrack()->hash)
            {
                if (!dot->getTrack()->playing)
                    leader = nullptr;
            }
            else if (dot->getTrack()->playing)
            {
                follower = dot;
            }
        }
        else if (dot->getTrack()->playing)
        {
            follower = dot;
        }
    }
    
    if (leader == nullptr)
    {
        leader = follower;
        follower = nullptr;
    }
    
    repaint();
}


void DirectionView::addAnalysed(TrackInfo* track)
{
    const juce::ScopedLock sl(lock);
    
    dots.add(new TrackDotComponent(track));
    calculatePositions();
    
    juce::MessageManager::callAsync(std::function<void()>([this]() {
        resized();
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
        
        // Prevent division by zero when there is only 1 track
        if (results.minBpm == results.maxBpm)
            xProportion = 0.5;
        else
            xProportion = float(info.bpm - results.minBpm) / (results.maxBpm - results.minBpm);
        
        // Do the same for groove on y-axis
        if (results.minGroove == results.maxGroove)
            yProportion = 0.5;
        else
            yProportion = float(info.groove - results.minGroove) / (results.maxGroove - results.minGroove);
        
        // Flip y axis so high groove is at top
        yProportion = 1 - yProportion;
        
        // Check that the values are within range
        jassert(xProportion >= 0 && xProportion <= 1 && yProportion >= 0 && yProportion <= 1);
        
        dot->setPosition(xProportion, yProportion);
    }
}


void DirectionView::reset()
{
    removeAllChildren();
    dots.clear();
    numDotsAdded = 0;
}
