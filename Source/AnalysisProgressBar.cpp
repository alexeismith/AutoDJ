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
    pauseImg = juce::ImageFileFormat::loadFrom(BinaryData::pause_png, BinaryData::pause_pngSize);
    
    playPauseBtn.reset(new juce::ImageButton());
    addChildComponent(playPauseBtn.get());
    playPauseBtn->setImages(false, true, true, pauseImg, 0.65f, {}, pauseImg, 1.f, {}, pauseImg, 1.f, {});
    playPauseBtn->setInterceptsMouseClicks(false, false);
}


void AnalysisProgressBar::resized()
{
    playPauseBtn->setSize(18, 18);
    playPauseBtn->setCentrePosition(getWidth()/2, getHeight()/2);
}


void AnalysisProgressBar::mouseEnter(const juce::MouseEvent &event)
{
    setTextToDisplay("");
    
    playPauseBtn->setVisible(true);
}


void AnalysisProgressBar::mouseDown(const juce::MouseEvent &event)
{
    analysisManager->playPause();
    
    paused = !paused;
    
    if (paused)
        playPauseBtn->setImages(false, true, true, playImg, 0.65f, {}, playImg, 1.f, {}, playImg, 1.f, {});
    else
        playPauseBtn->setImages(false, true, true, pauseImg, 0.65f, {}, pauseImg, 1.f, {}, pauseImg, 1.f, {});
}


void AnalysisProgressBar::mouseExit(const juce::MouseEvent &event)
{
    playPauseBtn->setVisible(false);
    
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
