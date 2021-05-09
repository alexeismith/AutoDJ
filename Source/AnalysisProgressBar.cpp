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
    setTextToDisplay("Analysing Library...");
    
    playPauseBtn.reset(new juce::TextButton("||"));
    addChildComponent(playPauseBtn.get());
    playPauseBtn->setInterceptsMouseClicks(false, false);
}


void AnalysisProgressBar::resized()
{
    playPauseBtn->setSize(25, 25);
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
        playPauseBtn->setButtonText(">");
    else
        playPauseBtn->setButtonText("||");
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
