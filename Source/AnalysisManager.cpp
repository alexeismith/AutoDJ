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
//        if (onsetResult[i] < 800) onsetResult[i] = 0;
    }
    
    int nonZeroCount = static_cast<int>(onsetResult.size());
    while (nonZeroCount > 0 && onsetResult.at(nonZeroCount - 1) <= 0.0) {
        --nonZeroCount;
    }
    
//    GraphComponent::store(onsetResult.data(), numFrames);

    std::vector<double> df;
    std::vector<double> beatPeriod;
    std::vector<double> tempi;
    const auto required_size = std::max(0, nonZeroCount - 2);
    df.reserve(required_size);
    beatPeriod.reserve(required_size);
    
    // skip first 2 results as it might have detect noise as onset
    // that's how vamp does and seems works best this way
    for (int i = 2; i < nonZeroCount; ++i) {
        df.push_back(onsetResult.at(i));
        beatPeriod.push_back(0.0);
    }

    TempoTrackV2 tt(SUPPORTED_SAMPLERATE, STEP_SIZE);
    tt.calculateBeatPeriod(df, beatPeriod, tempi);

    std::vector<double> beats;
    tt.calculateBeats(df, beatPeriod, beats);

//    std::vector<double> resultBeats;
//    resultBeats.reserve(static_cast<int>(beats.size()));
//    for (size_t i = 0; i < beats.size(); ++i) {
//        // we add the halve m_stepSize here, because the beat
//        // is detected between the two samples.
//        double result = (beats.at(i) * STEP_SIZE) + STEP_SIZE / 2;
//        resultBeats.push_back(result);
//    }

    onsetAnalyser.reset();
    
    DBG("DONE");
    
    DBG("tempi...");
    for (int i = 0; i < tempi.size(); i++) DBG(tempi[i]);
    
    DBG("beats...");
    for (int i = 0; i < beats.size(); i++) DBG(beats[i]);
    
    DBG("beatIntervals...");
    int prev = 0;
    for (int i = 0; i < beats.size(); i++)
    {
        DBG(beats[i] - prev);
        prev = beats[i];
    }
    
    
    
//    tempoTracker.calculateBeatPeriod(onsetResult, beatPeriod, tempi);
//
//    std::fill(beatPeriod.begin(), beatPeriod.end(), AutoDJ::mostCommonValue(beatPeriod.data(), (int)beatPeriod.size()));
//
//    tempoTracker.calculateBeats(onsetResult, beatPeriod, beats);
//
////    for (int i = 0; i < tempi.size(); i++) DBG(tempi[i]);
//
//    DBG("Most Common: " << AutoDJ::mostCommonValue(tempi.data(), (int)tempi.size()));
//    DBG("Most Common: " << AutoDJ::mostCommonValue(beatPeriod.data(), (int)beatPeriod.size()));

//    int prev = 0;
//    for (int i = 0; i < beats.size(); i++)
//    {
//        DBG(beats[i]);
//        DBG(beats[i] - prev);
//        prev = beats[i];
//    }
}
