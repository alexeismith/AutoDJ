//
//  AnalysisManager.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 26/03/2021.
//

#include "AnalysisManager.hpp"

#include "CommonDefs.hpp"

#include "GraphComponent.hpp"

#define STEP_SIZE (512) // Ideal for 44.1kHz sample rate (see https://code.soundsoftware.ac.uk/projects/qm-vamp-plugins/repository/entry/plugins/BarBeatTrack.cpp#L249)


AnalysisManager::AnalysisManager(TrackDataManager* dm) :
    dataManager(dm),
    tempoTracker(TempoTrackV2(SUPPORTED_SAMPLERATE, STEP_SIZE)),
    stBpmDetect(1, SUPPORTED_SAMPLERATE)
{
    dfConfig.DFType = DF_COMPLEXSD;
    dfConfig.stepSize = STEP_SIZE;
    dfConfig.frameLength = STEP_SIZE * 2; // See https://code.soundsoftware.ac.uk/projects/qm-vamp-plugins/repository/entry/plugins/BarBeatTrack.cpp#L258
    dfConfig.dbRise = 3;
    dfConfig.adaptiveWhitening = false;
    dfConfig.whiteningRelaxCoeff = -1;
    dfConfig.whiteningFloor = -1;
    onsetAnalyser.reset(new DetectionFunction(dfConfig));
}


void AnalysisManager::analyse(TrackData track)
{
    juce::AudioBuffer<float> buffer;
    juce::AudioBuffer<double> bufferDouble;
    
    dataManager->fetchAudio(track.filename, buffer, true);

    DBG("Copying");
    
    bufferDouble.setSize(1, buffer.getNumSamples());

    for (int i = 0; i < buffer.getNumSamples(); i++) {
        bufferDouble.setSample(0, i, (double)buffer.getSample(0, i));
    }

    DBG("Analysing");

    int numFrames = (buffer.getNumSamples() - dfConfig.frameLength) / dfConfig.stepSize;

    std::vector<double> onsetResult;
    onsetResult.resize(numFrames, 0);

    for (int i = 0; i < numFrames; i++) {
        onsetResult[i] = onsetAnalyser->processTimeDomain(bufferDouble.getReadPointer(0, i*dfConfig.stepSize));
    }
    
    GraphComponent::store(onsetResult.data(), numFrames);

    std::vector<double> beatPeriod;
    beatPeriod.resize(numFrames);
    std::vector<double> tempi;
    vector<double> beats;

    tempoTracker.calculateBeatPeriod(onsetResult, beatPeriod, tempi);

    tempoTracker.calculateBeats(onsetResult, beatPeriod, beats);

    DBG("TEMPI");
    
    for (int i = 0; i < tempi.size(); i++) DBG(tempi[i]);
    
    DBG("BEATS");
    
    for (int i = 0; i < beats.size(); i++) DBG(beats[i]);
    
    DBG("Analysing");
    
    int blockSize = 2048;
    int numBlocks = buffer.getNumSamples() / blockSize;
    
    for (int i = 0; i < numBlocks; i++)
    {
        stBpmDetect.inputSamples(buffer.getReadPointer(0, i*blockSize), blockSize);
    }
    
    DBG(stBpmDetect.getBpm());
    
    DBG("DONE");
}
