//
//  AnalyserEnergy.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 30/04/2021.
//

#include "AnalyserEnergy.hpp"


AnalyserEnergy::AnalyserEnergy()
{
    // register the algorithms in the factory(ies)
    essentia::init();

//    essentia::standard::Pool pool;

    /////// PARAMS //////////////
    int sampleRate = 44100;
    int frameSize = 2048;
    int hopSize = 1024;
}
