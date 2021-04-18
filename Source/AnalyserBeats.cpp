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
#define DOWNBEAT_DECIMATION_FACTOR (16)

AnalyserBeats::AnalyserBeats()
{
    dfConfig.DFType = DF_COMPLEXSD;
    dfConfig.stepSize = STEP_SIZE;
    dfConfig.frameLength = STEP_SIZE * 2; // See https://code.soundsoftware.ac.uk/projects/qm-vamp-plugins/repository/entry/plugins/BarBeatTrack.cpp#L258
    dfConfig.dbRise = 3;
    dfConfig.adaptiveWhitening = false;
    dfConfig.whiteningRelaxCoeff = -1;
    dfConfig.whiteningFloor = -1;
    
    downBeat.reset(new DownBeat(SUPPORTED_SAMPLERATE, DOWNBEAT_DECIMATION_FACTOR, STEP_SIZE));
    downBeat->setBeatsPerBar(BEATS_PER_BAR);
}


void AnalyserBeats::analyse(juce::AudioBuffer<float> audio, std::atomic<double>* progress, int& bpm, int& beatPhase, int& downbeat)
{
    reset();
    
    // Find the number of onset detection frames for the provided audio
    int numFrames = (audio.getNumSamples() - dfConfig.frameLength) / dfConfig.stepSize;
    
    getTempo(audio, progress, numFrames, bpm, beatPhase);
    
    progress->store(0.7);
    
    getDownbeat(audio, numFrames, bpm, beatPhase, downbeat);
}


void AnalyserBeats::reset()
{
    // Reset the QM onset and downbeat analysers
    onsetAnalyser.reset(new DetectionFunction(dfConfig));
    downBeat->resetAudioBuffer();
}


void AnalyserBeats::getTempo(juce::AudioBuffer<float> audio, std::atomic<double>* progress, int numFrames, int& bpm, int& beatPhase)
{
    juce::AudioBuffer<double> buffer;
    std::vector<double> onsets, onsetsTrim, beatPeriod, tempi, beats;
    
    // Analysis classes require double, so copy audio into a double buffer
    buffer.setSize(1, audio.getNumSamples());
    for (int i = 0; i < audio.getNumSamples(); i++)
        buffer.setSample(0, i, (double)audio.getSample(0, i));
    
    // Allocate buffer space for the onset results
    onsets.reserve(numFrames);

    // Pass frames of audio to the QM onset detector, storing the returned results
    for (int i = 0; i < numFrames; i++)
    {
        onsets.push_back(onsetAnalyser->processTimeDomain(buffer.getReadPointer(0, i*dfConfig.stepSize)));
        progress->store(0.1 + 0.5 * (double(i) / numFrames));
    }
    
    // Find the number of non-zero values in the onset results
    int nonZeroCount = int(onsets.size()); // Initialise count with the full vector length
    while (nonZeroCount > 0 && onsets.at(nonZeroCount - 1) <= 0.0)
        --nonZeroCount; // For every zero at the back of the vector, decrement the counter

    // Allocate memory for the trimmed results
    int required_size = std::max(0, nonZeroCount - 0);
    onsetsTrim.reserve(required_size);
    beatPeriod.reserve(required_size);
    
    // Trim the onset results and prepare the beat period vector
    // We trim the first 2 elements (to account for unstable onset analysis at start), and the trailing zeros
    for (int i = 0; i < nonZeroCount; ++i)
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
    processBeats(beats, bpm, beatPhase);
}


void AnalyserBeats::processBeats(std::vector<double> beats, int& bpm, int& beatPhase)
{
    std::vector<BeatUtils::ConstRegion> constantRegions = BeatUtils::retrieveConstRegions(beats, SUPPORTED_SAMPLERATE);

    double firstBeat = 0;
    bpm = BeatUtils::makeConstBpm(constantRegions, SUPPORTED_SAMPLERATE, &firstBeat);
    firstBeat = BeatUtils::adjustPhase(firstBeat, bpm, SUPPORTED_SAMPLERATE, beats);
    
    // 'Rewind' firstBeat to start of track
    double beatLength = 60 * SUPPORTED_SAMPLERATE / bpm;
    beatPhase = firstBeat - floor(firstBeat / beatLength) * beatLength;
}


void AnalyserBeats::getDownbeat(juce::AudioBuffer<float> audio, int numFrames, int bpm, int beatPhase, int& downbeat)
{
    std::vector<double> beats;
    
    for (int i = 0; i < numFrames; i++)
    {
        if (isBeat(i, bpm, beatPhase))
        {
            beats.push_back(i);
        }
    }
    
    for (int i = 0; i < numFrames; i++)
        downBeat->pushAudioBlock(audio.getReadPointer(0, i*dfConfig.stepSize));
    
    vector<int> downbeats;
    size_t downLength = 0;
    const float *downsampled = downBeat->getBufferedAudio(downLength);
    downBeat->findDownBeats(downsampled, downLength, beats, downbeats);
    
    downbeat = downbeats.front();

    // The following can be used to fetch the spectral difference for each beat
//    vector<double> beatsd;
//    downBeat->getBeatSD(beatsd);
}


bool AnalyserBeats::isBeat(int frame, int bpm, int beatPhase)
{
    int frameStart = frame * STEP_SIZE;
    int frameEnd = frameStart + STEP_SIZE - 1;
    
    double beatLength = round(60 * SUPPORTED_SAMPLERATE / bpm);
    
    frameStart -= beatPhase;
    frameEnd -= beatPhase;
    
    if (abs(floor(frameEnd/beatLength) - floor(frameStart/beatLength)) > 0)
        return true;
    
    return false;
}
