//
//  PerformanceMeasure.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 13/05/2021.
//

#ifndef PerformanceMeasure_hpp
#define PerformanceMeasure_hpp

#define PERFORMANCE_START double timeSec; \
{ \
juce::ScopedTimeMeasurement m(timeSec);


#define PERFORMANCE_END } \
PerformanceMeasure::addResult(timeSec);


class PerformanceMeasure
{
public:
    
    PerformanceMeasure() {}
    
    ~PerformanceMeasure() {}
    
    static void addResult(double measurement);

    static double getAverage();
    
    static void reset();
    
};

#endif /* PerformanceMeasure_hpp */
