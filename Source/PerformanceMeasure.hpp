//
//  PerformanceMeasure.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 13/05/2021.
//

#ifndef PerformanceMeasure_hpp
#define PerformanceMeasure_hpp

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
