//
//  InterpolatedParameter.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 21/04/2021.
//

#ifndef InterpolatedParameter_hpp
#define InterpolatedParameter_hpp


/**
 Represents a parameter that can gradually modulate between values.
 Can set a current and target value, number of samples over which the modulation should occur,
 as well as a point at which to start the modulation.
 */
class InterpolatedParameter
{
public:
    
    /** Constructor. */
    InterpolatedParameter() {}
    
    /** Destructor. */
    ~InterpolatedParameter() {}
    
    /** Updates the parameter value, based on the number of track samples processed.
     
     @param[in] currentSample Playhead position in track
     @param[in] numSamples Number of audio samples processed since last update */
    void update(int currentSample, int numSamples);
    
    // Set startSample < 0 for instant change now
    // Set numSamples <= 0 for instant change at startSample
    /** Specifies a new interpolation.
     
     @param[in] targetValue Parameter value to reach by end of modulation period
     @param[in] startSample Position in track to start the modulation, in audio samples
     @param[in] numSamples Length of modulation period, in audio samples*/
    void moveTo(double targetValue, int startSample = -1, int numSamples = -1);
    
    /** Resets the current parameter value.
     
     @param[in] value New value for the parameter */
    void resetTo(double value);
    
    double currentValue = 0.0; ///< Current value of the parameter
    double targetValue = 0.0; ///< Target value that the parameter will reach after interpolation
    double rateOfChange = 0.0; ///< Amount the value will change per audio sample
    int startSample = -1; ///< Position in track to start the modulation, in audio samples
    int samplesRemaining = -1; ///< Number of audio samples remaining before interpolation is complete
    
};

#endif /* InterpolatedParameter_hpp */
