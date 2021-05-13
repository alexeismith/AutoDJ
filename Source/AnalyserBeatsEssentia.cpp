//
//  AnalyserBeatsEssentia.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 01/05/2021.
//

#include "AnalyserBeatsEssentia.hpp"

#include "CommonDefs.hpp"

#include "ThirdParty/beatutils.h"

#include "PerformanceMeasure.hpp"

// TODO: define these elsewhere
#define MIN_TEMPO (90)
#define MAX_TEMPO (160)


#define STEP_SIZE (512)
#define FRAME_LENGTH (STEP_SIZE*2)
#define DOWNBEAT_DECIMATION_FACTOR (16)

AnalyserBeatsEssentia::AnalyserBeatsEssentia(essentia::standard::AlgorithmFactory& factory)
{
    rhythmExtractor.reset(factory.create("RhythmExtractor2013", "minTempo", MIN_TEMPO, "maxTempo", MAX_TEMPO, "method", "degara"));
    
    downBeat.reset(new DownBeat(SUPPORTED_SAMPLERATE, DOWNBEAT_DECIMATION_FACTOR, STEP_SIZE));
    downBeat->setBeatsPerBar(BEATS_PER_BAR);
}


void AnalyserBeatsEssentia::analyse(juce::AudioBuffer<float>* audio, std::atomic<double>* progress, int& bpm, int& beatPhase, int& downbeat)
{
    reset();
    
    int numFrames = (audio->getNumSamples() - FRAME_LENGTH) / STEP_SIZE;
    
    getTempo(audio, progress, numFrames, bpm, beatPhase);
    
    getDownbeat(audio, numFrames, bpm, beatPhase, downbeat);
}


void AnalyserBeatsEssentia::reset()
{
    downBeat->resetAudioBuffer();
}


void AnalyserBeatsEssentia::getTempo(juce::AudioBuffer<float>* audio, std::atomic<double>* progress, int numFrames, int& bpm, int& beatPhase)
{
    std::vector<float> buffer(audio->getReadPointer(0), audio->getReadPointer(0) + audio->getNumSamples());
    std::vector<float> ticks;
    std::vector<float> estimates;
    std::vector<float> bpmIntervals;
    std::vector<double> beats;
    
    float bpmFloat;
    float confidence;
    
    double timeSec;
    {
    juce::ScopedTimeMeasurement m(timeSec);
    
    rhythmExtractor->reset();
    
    try {
        rhythmExtractor->input("signal").set(buffer);
        
        rhythmExtractor->output("bpm").set(bpmFloat);
        rhythmExtractor->output("ticks").set(ticks);
        rhythmExtractor->output("confidence").set(confidence);
        rhythmExtractor->output("estimates").set(estimates);
        rhythmExtractor->output("bpmIntervals").set(bpmIntervals);

        rhythmExtractor->compute();
    }
    catch (std::exception& e) {
        DBG(((essentia::EssentiaException&)e).what());
    }
    
    bpm = round(bpmFloat);
    
    }
    
    PerformanceMeasure::addResult(timeSec);
    
//    DBG("bpm: " << bpmFloat << " confidence: " << confidence);
    
    
//    for (auto tick : ticks)
//        beats.push_back(tick*SUPPORTED_SAMPLERATE);
//
//    processBeats(beats, bpm, beatPhase);
    
//    DBG("bpm: " << bpm << " beatPhase: " << beatPhase);
    
    // Manual phase construction method...
//    int beatPeriod = AutoDJ::getBeatPeriod(round(bpmFloat));
//    
//    int phaseAvg = 0;
//    
//    for (int i = 10; i < ticks.size(); i++)
//    {
//        phaseAvg += int(round(ticks.at(i)*SUPPORTED_SAMPLERATE)) % beatPeriod;
//    }
//    
//    phaseAvg /= ticks.size() - 10;
//    
//    DBG("phaseAvg: " << phaseAvg);
//    
//    beatPhase = phaseAvg;
}


void AnalyserBeatsEssentia::processBeats(std::vector<double> beats, int& bpm, int& beatPhase)
{
    std::vector<BeatUtils::ConstRegion> constantRegions = BeatUtils::retrieveConstRegions(beats, SUPPORTED_SAMPLERATE);

    double firstBeat = 0;
    bpm = BeatUtils::makeConstBpm(constantRegions, SUPPORTED_SAMPLERATE, &firstBeat);
    firstBeat = BeatUtils::adjustPhase(firstBeat, bpm, SUPPORTED_SAMPLERATE, beats);
    
    // 'Rewind' firstBeat to start of track
    double beatLength = 60 * SUPPORTED_SAMPLERATE / bpm;
    beatPhase = firstBeat - floor(firstBeat / beatLength) * beatLength;
}


void AnalyserBeatsEssentia::getDownbeat(juce::AudioBuffer<float>* audio, int numFrames, int bpm, int beatPhase, int& downbeat)
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
        downBeat->pushAudioBlock(audio->getReadPointer(0, i*STEP_SIZE));
    
    std::vector<int> downbeats;
    size_t downLength = 0;
    const float *downsampled = downBeat->getBufferedAudio(downLength);
    downBeat->findDownBeats(downsampled, downLength, beats, downbeats);
    
    downbeat = downbeats.front();

    // The following can be used to fetch the spectral difference for each beat
//    std::vector<double> beatsd;
//    downBeat->getBeatSD(beatsd);
}


bool AnalyserBeatsEssentia::isBeat(int frame, int bpm, int beatPhase)
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
