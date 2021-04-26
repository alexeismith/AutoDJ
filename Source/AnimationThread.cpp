//
//  AnimationThread.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 26/04/2021.
//

#include "AnimationThread.hpp"


void AnimationThread::update(int deckId, int playhead, double timeStretch, double gain)
{
    AnimationInfo* info;
    if (deckId == 0)
        info = &info1;
    else
        info = &info2;
    
    info->playhead.store(playhead);
    info->timeStretch.store(timeStretch);
    info->gain.store(gain);
}


void hiResTimerCallback()
{
//    wav
}
