//
//  TrackChooser.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 28/04/2021.
//

#include "TrackChooser.hpp"


TrackChooser::TrackChooser(TrackDataManager* dm)
{
    dataManager = dm;
    sorter = dataManager->getSorter();
}


TrackInfo TrackChooser::chooseTrack(MixDirection direction)
{
    bool random = true;
    
    int randomChoice;
    TrackInfo* track = nullptr;
    
    // TODO: handle case where all tracks have been played, currrently infinite loop
    // Use separate unplayed list, which keeps updated with the normal one
    
    if (random)
    {
        do
        {
            randomChoice = rand() % dataManager->getTracks()->size();
            track = &dataManager->getTracks()->getReference(randomChoice);
        } while (!track->analysed || track->queued);
//        } while (!track->analysed); TODO: remove
    }
    else
    {
        // TODO: Take into account BPM, Key, Energy
    }
    
//    DBG("QUEUED: " << track->filename);
    track->queued = true;
    
    return *track;
}
