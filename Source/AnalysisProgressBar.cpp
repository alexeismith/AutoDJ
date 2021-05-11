//
//  AnalysisProgressBar.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 05/05/2021.
//

#include "AnalysisProgressBar.hpp"


AnalysisProgressBar::AnalysisProgressBar(AnalysisManager* am) :
    juce::ProgressBar(progress), analysisManager(am)
{
    setColour(juce::ProgressBar::ColourIds::backgroundColourId, getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).withBrightness(0.15f));
    
    setTextToDisplay("Analysing Library...");
    
    playImg = juce::ImageFileFormat::loadFrom(BinaryData::play_png, BinaryData::play_pngSize);
    playImg.multiplyAllAlphas(0.65f);
    
    pauseImg = juce::ImageFileFormat::loadFrom(BinaryData::pause_png, BinaryData::pause_pngSize);
    pauseImg.multiplyAllAlphas(0.65f);
}


void AnalysisProgressBar::paint(juce::Graphics& g)
{
    juce::ProgressBar::paint(g);
    
    if (mouseOver)
    {
        if (paused)
            g.drawImage(playImg, imageArea);
        else
            g.drawImage(pauseImg, imageArea);
    }
}


void AnalysisProgressBar::resized()
{
    imageArea.setSize(18, 18);
    imageArea.setCentre(getWidth()/2, getHeight()/2);
}


void AnalysisProgressBar::mouseEnter(const juce::MouseEvent &event)
{
    setTextToDisplay("");
    
    mouseOver = true;
}


void AnalysisProgressBar::mouseDown(const juce::MouseEvent &event)
{
    playPause();
}


void AnalysisProgressBar::mouseExit(const juce::MouseEvent &event)
{
    mouseOver = false;
    
    if (paused)
        setTextToDisplay("Analysis Paused.");
    else
        setTextToDisplay("Analysing Library...");
}


void AnalysisProgressBar::update(double p)
{
    if (!paused)
        progress = p;
}


void AnalysisProgressBar::playPause()
{
    const juce::ScopedLock sl(lock);
    
    paused = !paused;
    
    analysisManager->playPause();
    
    repaint();
}

void AnalysisProgressBar::pause()
{
    const juce::ScopedLock sl(lock);
    
    if (!paused)
        playPause();
}
