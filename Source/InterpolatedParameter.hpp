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
    
    InterpolatedParameter(double currentValue, double targetValue, int numSamples = -1);
    
    ~InterpolatedParameter() {}
    
    void update(int numSamples);
    
    double currentValue;
    double targetValue;
    double rateOfChange;
    int samplesRemaining = -1;
};

#endif /* InterpolatedParameter_hpp */
