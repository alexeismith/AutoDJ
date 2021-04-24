//
//  MixView.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 24/04/2021.
//

#include "MixView.hpp"

#include "CommonDefs.hpp"


MixView::MixView(TrackDataManager* dm) :
    dataManager(dm)
{
    waveform.reset(new WaveformComponent(800, dataManager));
    addAndMakeVisible(waveform.get());
    waveform->setVisible(false);
}


void MixView::resized()
{
    waveform->setSize(getWidth(), 100);
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
