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
    
    for (int i = 0; i < 1000; i++)
        updateVelocity();
}


TrackInfo TrackChooser::chooseTrackComplex()
{
    
}


TrackInfo TrackChooser::chooseTrackRandom()
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


void TrackChooser::updateVelocity()
{
    const double momentum = 0.9;
    std::normal_distribution<double> distribution(0, 0.5);
    
    accelerationBpm = accelerationBpm*momentum + distribution(randomGenerator)*(1.0 - momentum);
    accelerationKey = accelerationKey*momentum + distribution(randomGenerator)*(1.0 - momentum);
    
    velocityBpm += accelerationBpm;
    velocityKey += accelerationKey;

    velocityBpm = juce::jmin(velocityBpm, 1.0);
    velocityKey = juce::jmin(velocityKey, 1.0);
    
    velocityBpm = juce::jmax(velocityBpm, -1.0);
    velocityKey = juce::jmax(velocityKey, -1.0);
    
    DBG("velocityBpm: " << velocityBpm << " velocityKey: " << velocityKey);
}
