//
//  TrackEditor.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 06/05/2021.
//

#include "TrackEditor.hpp"


TrackEditor::TrackEditor()
{
    waveform.reset(new WaveformView(true, false));
}


void TrackEditor::load(Track* t)
{
    track = t;
    waveform->load(track);
}
