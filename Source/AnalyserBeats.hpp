//
//  AnalyserBeats.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 11/04/2021.
//

#ifndef AnalyserBeats_hpp
#define AnalyserBeats_hpp

#include <JuceHeader.h>
#include "ThirdParty/qm-dsp/dsp/tempotracking/TempoTrackV2.h"
#include "ThirdParty/qm-dsp/dsp/tempotracking/DownBeat.h"
#include "ThirdParty/qm-dsp/dsp/onsets/DetectionFunction.h"


/**
 Temporal MIR analyser that uses QM-DSP algorithms to extract tempo, beat phase and downbeat position.
 Implementation is inspired by: https://github.com/c4dm/qm-vamp-plugins/blob/master/plugins/BarBeatTrack.cpp
 
 Beat tracking algorithm is based on: https://www.researchgate.net/publication/3457779_Context-Dependent_Beat_Tracking_of_Musical_Audio
 Downbeat algorithm is based on: https://ieeexplore.ieee.org/document/7071189
 */
class AnalyserBeats
{
public:
    
    /** Constructor. */
    AnalyserBeats();
    
    /** Destructor. */
    ~AnalyserBeats() {}
    
    /** Analyses the provided audio data.
     
     @param[in] audio Pointer to audio data to be analysed
     @param[out] progress Variable in which to store analysis progress
     @param[out] bpm Output location for tempo result
     @param[out] beatPhase Output location for beat phase result
     @param[out] downbeat  Output location for downbeat result */
    void analyse(juce::AudioBuffer<float>* audio, std::atomic<double>* progress, int& bpm, int& beatPhase, int& downbeat);
    
private:
    
    /** Resets the analyser ready for a new track. */
    void reset();
    
    /** Performs beat tracking to extract tempo and beat phase.
     
     @param[in] audio Pointer to audio data to be analysed
     @param[out] progress Variable in which to store analysis progress
     @param[in] numFrames Number of frames to be output by the onset detection function
     @param[out] bpm Output location for tempo result
     @param[out] beatPhase Output location for beat phase result */
    void getTempo(juce::AudioBuffer<float>* audio, std::atomic<double>* progress, int numFrames, int& bpm, int& beatPhase);
    
    /** Determines an overall tempo and beat phase from the provided beat grid,
     by finding the values which are dominnat across the constant sections of the beat grid.
     This is called by getTempo() after beat tracking is performed.
     
     @param[in] beats Beat grid (array of beat positions)
     @param[out] bpm Output location for tempo result
     @param[out] beatPhase Output location for beat phase result */
    void processBeats(std::vector<double> beats, int& bpm, int& beatPhase);
    
    /** Find the downbeat position in the provided audio, based on its tempo and beat phase results.
     
     @param[in] audio Pointer to audio data to be analysed
     @param[in] numFrames Number of frames which were output by the onset detection function
     @param[in] bpm Tempo result, in beats-per-minute
     @param[in] beatPhase Beat phase result, in audio samples
     @param[out] downbeat  Output location for downbeat result */
    void getDownbeat(juce::AudioBuffer<float>* audio, int numFrames, int bpm, int beatPhase, int& downbeat);
    
    /** Checks whether a given onset signal frame contains a beat.
     Used for constructing a beat grid based on the tempo and beat phase results.
     
     @param[in] frame Index of the onset signal frame
     @param[in] bpm Tempo result, in beats-per-minute
     @param[in] beatPhase Beat phase result, in audio samples
     
     @return True if the frame contains a beat */
    bool isBeat(int frame, int bpm, int beatPhase);
    
    std::unique_ptr<DetectionFunction> onsetAnalyser; ///< QM-DSP onset detection function (the actual beat tracker, TempoTrackV2, is local to the getTempo() function)
    DFConfig dfConfig; ///< Parameters for the QM-DSP onset detection function
    
    std::unique_ptr<DownBeat> downBeat; ///< QM-DSP downbeat detector
    
    juce::IIRFilter filter; ///< Low-pass filter (unused in normal config - see BeatTests.hpp)
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalyserBeats) ///< JUCE macro to add a memory leak detector
};

#endif /* AnalyserBeats_hpp */
