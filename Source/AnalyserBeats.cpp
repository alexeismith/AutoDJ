//
//  AnalyserBeats.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 11/04/2021.
//

#include "AnalyserBeats.hpp"

#include "CommonDefs.hpp"

#include "ThirdParty/beatutils.h"

#define STEP_SIZE (512) // Ideal for 44.1kHz sample rate (see https://code.soundsoftware.ac.uk/projects/qm-vamp-plugins/repository/entry/plugins/BarBeatTrack.cpp#L249)


AnalyserBeats::AnalyserBeats() :
    tempoTracker(TempoTrackV2(SUPPORTED_SAMPLERATE, STEP_SIZE))
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


void AnalyserBeats::analyse(juce::AudioBuffer<float> audio, int& bpm, int& beatPhase, int& downbeat)
{
    juce::AudioBuffer<double> buffer;
    std::vector<double> onsets, onsetsTrim, beatPeriod, tempi, beats;
    
    // Analysis classes require double, so copy audio into a double buffer
    buffer.setSize(1, audio.getNumSamples());
    for (int i = 0; i < audio.getNumSamples(); i++)
        buffer.setSample(0, i, (double)audio.getSample(0, i));

    // Find the number of onset detection frames for the provided audio
    int numFrames = (audio.getNumSamples() - dfConfig.frameLength) / dfConfig.stepSize;
    onsets.reserve(numFrames); // Allocate buffer space for the onset results

    // Pass frames of audio to the QM onset detector, storing the returned results
    for (int i = 0; i < numFrames; i++)
        onsets.push_back(onsetAnalyser->processTimeDomain(buffer.getReadPointer(0, i*dfConfig.stepSize)));
    
    // Find the number of non-zero values in the onset results
    int nonZeroCount = int(onsets.size()); // Initialise count with the full vector length
    while (nonZeroCount > 0 && onsets.at(nonZeroCount - 1) <= 0.0)
        --nonZeroCount; // For every zero at the back of the vector, decrement the counter

    // Allocate memory for the trimmed results
    int required_size = std::max(0, nonZeroCount - 2);
    onsetsTrim.reserve(required_size);
    beatPeriod.reserve(required_size);
    
    // Trim the onset results and prepare the beat period vector
    // We trim the first 2 elements (to account for unstable onset analysis at start), and the trailing zeros
    for (int i = 2; i < nonZeroCount; ++i)
    {
        onsetsTrim.push_back(onsets.at(i));
        beatPeriod.push_back(0.0);
    }
    
    // Prepare the QM beat tracker
    TempoTrackV2 tt(SUPPORTED_SAMPLERATE, STEP_SIZE);

    // Pass the onset results and other empty vectors to the QM beat tracker functions
    tt.calculateBeatPeriod(onsetsTrim, beatPeriod, tempi);
    tt.calculateBeats(onsetsTrim, beatPeriod, beats);

    // Multiply beat positions by STEP_SIZE to convert back to 44.1kHz sample rate
    for (int i = 0; i < beats.size(); i++)
        beats[i] *= STEP_SIZE;
    
    // Adjust the non-constant beat tracking results to get a constant BPM and beat phase
    processBeats(beats, bpm, beatPhase, downbeat);
    
    // Reset the QM onset detector
    onsetAnalyser.reset();
}


void AnalyserBeats::processBeats(std::vector<double> beats, int& bpm, int& beatPhase, int& downbeat)
{
    std::vector<BeatUtils::ConstRegion> constantRegions = BeatUtils::retrieveConstRegions(beats, SUPPORTED_SAMPLERATE);

    double firstBeat = 0;
    bpm = BeatUtils::makeConstBpm(constantRegions, SUPPORTED_SAMPLERATE, &firstBeat);
    beatPhase = BeatUtils::adjustPhase(firstBeat, bpm, SUPPORTED_SAMPLERATE, beats);
    
    DBG("constBPM: " << bpm << " firstBeat: " << beatPhase);
}
