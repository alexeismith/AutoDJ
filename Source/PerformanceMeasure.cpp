//
//  PerformanceMeasure.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 13/05/2021.
//

#include "PerformanceMeasure.hpp"

double measurementSum = 0.0;

int numMeasurements = 0;


void PerformanceMeasure::addResult(double measurement)
{
    measurementSum += measurement;
    numMeasurements += 1;
}


double PerformanceMeasure::getAverage()
{
    return measurementSum / numMeasurements;
}


void PerformanceMeasure::reset()
{
    measurementSum = 0.0;
    numMeasurements = 0;
}
