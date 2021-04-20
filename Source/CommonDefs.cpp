//
//  CommonDefs.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 06/03/2021.
//

#include "CommonDefs.hpp"

namespace AutoDJ {


juce::String getLengthString(int secs)
{
    if (secs % 60 < 10)
        return juce::String(floor(secs / 60)) + ":0" + juce::String(secs % 60);
    else
        return juce::String(floor(secs / 60)) + ":" + juce::String(secs % 60);
}


juce::String getKeyName(int chromaKey)
{
    juce::String name;
    int tonic = chromaKey;
    
    if (tonic > 12)
        tonic -= 12;
    
    switch(tonic)
    {
        case 1:
            name = juce::String("C");
            break;
        case 2:
            name = juce::String("Db");
            break;
        case 3:
            name = juce::String("D");
            break;
        case 4:
            name = juce::String("Eb");
            break;
        case 5:
            name = juce::String("E");
            break;
        case 6:
            name = juce::String("F");
            break;
        case 7:
            name = juce::String("Gb");
            break;
        case 8:
            name = juce::String("G");
            break;
        case 9:
            name = juce::String("Ab");
            break;
        case 10:
            name = juce::String("A");
            break;
        case 11:
            name = juce::String("Bb");
            break;
        case 12:
            name = juce::String("B");
            break;
        case -1:
            name = juce::String("-");
            break;
        default:
            jassert(false); // chromaKey not recognised
    }
    
    if (chromaKey > 12)
        name.append(juce::String("m"), 2);
    
    return name;
}


} /* namespace AutoDJ */
