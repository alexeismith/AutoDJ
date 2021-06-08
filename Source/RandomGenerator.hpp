//
//  RandomGenerator.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 07/05/2021.
//

#ifndef RandomGenerator_hpp
#define RandomGenerator_hpp

#include <random>

class RandomGenerator
{
public:
    
    /** Constructor. */
    RandomGenerator();
    
    /** Destructor. */
    ~RandomGenerator() {}
    
    double getGaussian(double stdDev = 1.0, double rangeLimit = -1.0, double shift = 0.0);
    
    bool getBool();
    
    int getInt(int min, int max);
    
private:
    
    std::mt19937 algorithm;

};

#endif /* RandomGenerator_hpp */
