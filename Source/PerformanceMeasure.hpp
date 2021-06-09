//
//  PerformanceMeasure.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 13/05/2021.
//

#ifndef PerformanceMeasure_hpp
#define PerformanceMeasure_hpp


// Simply include this file, and then use the following macros to take a time measurement.
// The result will automatically be added to PerformanceMeasure

// Line 1 (after 'PERFORMANCE_START'): defines a local variable in which to store the time measurement
// Line 2: Opens a new scope in which the ScopedTimeMeasurement will exist (see https://docs.juce.com/master/classScopedTimeMeasurement.html)
// Line 3: Starts the measurement by instantiating a ScopedTimeMeasurement
#define PERFORMANCE_START double timeSec; \
{ \
juce::ScopedTimeMeasurement m(timeSec);

// Line 1 (after 'PERFORMANCE_END'): Closes the scope in which the ScopedTimeMeasurement exists, which stops the measurement
// Line 2: Adds the result to the PerformanceMeasure average
#define PERFORMANCE_END } \
PerformanceMeasure::addResult(timeSec);


/**
 Stores time measurements used for testing algorithm performance.
 Use the PERFORMANCE_START and PERFORMANCE_END macros to take a measurement and add it to this class.
 Then, use getAverage() to fetch the average of the measurements.
 */
class PerformanceMeasure
{
public:
    
    /** Constructor. */
    PerformanceMeasure() {}
    
    /** Destructor. */
    ~PerformanceMeasure() {}
    
    /** Adds a time measurement to the overall result, which is stored in PerformanceMeasure.cpp. */
    static void addResult(double measurement);

    /** Fetches the average of the time measurements.
     
     @return Overall average value, in seconds */
    static double getAverage();
    
    /** Resets the averaging, ready for a new set of measurements. */
    static void reset();
    
};

#endif /* PerformanceMeasure_hpp */
