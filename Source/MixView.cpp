//
//  MixView.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/04/2021.
//

#include "MixView.hpp"

#include "CommonDefs.hpp"


MixView::MixView(TrackProcessor** processors)
{
    decks.add(new DeckComponent(0, processors[0]));
    decks.add(new DeckComponent(1, processors[1]));
    addAndMakeVisible(decks.getUnchecked(0));
    addAndMakeVisible(decks.getUnchecked(1));
    
    startTimer(33); // 10ms interval = 100Hz, 15ms = 66.7Hz, 33ms = 30.3Hz
}


void MixView::hiResTimerCallback()
{
    decks.getUnchecked(0)->update();
    decks.getUnchecked(1)->update();
    
    decks.getUnchecked(0)->flipWaveform();
    decks.getUnchecked(1)->flipWaveform();
    
    juce::MessageManager::callAsync(std::function<void()>([this]() {
        this->repaint();
//        this->decks.getUnchecked(0)->waveform->repaint();
//        this->decks.getUnchecked(1)->waveform->repaint();
    }));
}


void MixView::resized()
{
    decks.getUnchecked(0)->setSize(getWidth(), getHeight()/2);
    decks.getUnchecked(1)->setSize(getWidth(), getHeight()/2);
    
    decks.getUnchecked(0)->setTopLeftPosition(0, 0);
    decks.getUnchecked(1)->setTopLeftPosition(0, getHeight()/2);
}


void MixView::buttonClicked(juce::Button* button)
{
    int id = button->getComponentID().getIntValue();
    
    switch (id)
    {
        default:
            jassert(false); // Unrecognised button ID
    }
}


void MixView::paintOverChildren(juce::Graphics &g)
{
    g.setColour(juce::Colours::lightblue);
    g.drawVerticalLine(getWidth()/2, getHeight()/2 - WAVEFORM_HEIGHT - 10, getHeight()/2 + WAVEFORM_HEIGHT + 10);
}
