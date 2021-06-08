//
//  RandomGenerator.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 07/05/2021.
//

#ifndef RandomGenerator_hpp
#define RandomGenerator_hpp

#include <random>


/**
 Random number engine, which can generate random integers, booleans, and Gaussian-weighted doubles.
 */
class RandomGenerator
{
public:
    
    /** Constructor. */
    RandomGenerator();
    
    /** Destructor. */
    ~RandomGenerator() {}
    
    /** Generates a random double within the provided range, using a normal (Gaussian) distribution.
     
     @param[in] stdDev Standard deviation to be used for the normal distribution
     @param[in] rangeLimit Range limit for the normal distribution, to be applied in positive AND negative directions
     @param[in] shift Shift to be applied to value, AFTER limiting step
     
     @return Result of generation */
    double getGaussian(double stdDev = 1.0, double rangeLimit = -1.0, double shift = 0.0);
    
    /** Generates a random boolean (true or false, equally weighted). */
    bool getBool();
    
    /** Generates a random integer, equally weighted across the provided range.
     
     @param[in] min Minimum value that could be generated
     @param[in] max Maximum value that could be generated */
    int getInt(int min, int max);
    
private:
    
    std::mt19937 algorithm; ///< Algorithm used for generation of random numbers

};

#endif /* RandomGenerator_hpp */
