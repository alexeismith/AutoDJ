//
//  InterpolatedParameter.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 21/04/2021.
//

#ifndef InterpolatedParameter_hpp
#define InterpolatedParameter_hpp

class InterpolatedParameter
{
public:
    
    InterpolatedParameter() {}
    
    ~InterpolatedParameter() {}
    
    void update(int currentSample);
    
    // Set startSample < 0 for instant change now
    // Set numSamples <= 0 for instant change at startSample
    void moveTo(double targetValue, int startSample = -1, int numSamples = -1);
    
    double currentValue = 0.0;
    double targetValue = 0.0;
    double rateOfChange = 0.0;
    int startSample = -1;
    int samplesRemaining = -1;
};

#endif /* InterpolatedParameter_hpp */
