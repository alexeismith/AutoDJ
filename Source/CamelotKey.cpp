//
//  CamelotKey.cpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 09/05/2021.
//

#include "CamelotKey.hpp"


int CamelotKey::compability(CamelotKey key)
{
    jassert(value > -1); // Key must be initialised before calling this function
    
    // See the following page for a guide on how camelot harmonic mixing works:
    // https://mixedinkey.com/harmonic-mixing-guide/
    
    // Scoring:
    // 2: keys are the same, 1: keys are not equal, but still compatible, 0: keys are not compatible
    
    if (key.value == value) // Keys have same tonic
    {
        if (key.major == major) // Keys are the same - very compatible
            return 2;
        else // Keys have same tonic, different modes - compatible
            return 1;
    }
    else if (key.major == major) // Keys have same mode (not necessarily major)
    {
        if (abs(key.value - value) == 1) // Keys are adjacent on Camelot wheel - compatible
            return 1;
    }
    
    // Keys are not the same, and not adjacent - not compatible
    return 0;
}


int CamelotKey::sortByName(CamelotKey first, CamelotKey second)
{
    // Sorts the two provided keys in terms of their names...
    
    // Ascending Camelot number...
    if (first.value < second.value)
        return -1;
    else if (first.value > second.value)
        return 1;
    
    // Minor before major (for the same tonic)...
    if (!first.major && second.major)
        return -1;
    else if (first.major && !second.major)
        return 1;
    
    return 0;
}


juce::String CamelotKey::getName()
{
    if (value == -1)
        return juce::String("-");
    
    if (major)
        return juce::String(value) + "B";
    else
        return juce::String(value) + "A";
}


juce::Colour CamelotKey::getColour()
{
    if (value == -1)
        return juce::Colour();
    
    // Initialise colour as light blue, which is at the top of the camelot wheel:
    // https://mixedinkey.com/harmonic-mixing-guide/
    juce::Colour colour = juce::Colours::blue.withLightness(0.7);
    
    // Find how far round the camelot wheel this key is
    float rotation = float(value) / 12.f;
    
    if (value == 12)
        colour = colour.interpolatedWith(juce::Colours::white, 0.4f);
    
    // Rotate the colour hue by this amount and return
    return colour.withRotatedHue(1.f - rotation);
}


void CamelotKey::fromChromaKey(int chromaKey)
{
    // Convert a chromatic key number to Camelot representation...
    
    //
    
    // Chromatic key is in the range 1-24, where 1-12 is major and 13-24 is minor
    if (chromaKey <= 12)
        major = true;
    else
        major = false;
    
    // The tonic notes ascend chromatically within those ranges, e.g: 1 = C, 2 = Db, 13 = Cm, 14 = Dbm
    // Map the chromatic note number to Camelot number, using the wheel as reference: https://mixedinkey.com/harmonic-mixing-guide/
    switch(chromaKey)
    {
        case 1:
            value = 8;
            break;
        case 2:
            value = 3;
            break;
        case 3:
            value = 10;
            break;
        case 4:
            value = 5;
            break;
        case 5:
            value = 12;
            break;
        case 6:
            value = 7;
            break;
        case 7:
            value = 2;
            break;
        case 8:
            value = 9;
            break;
        case 9:
            value = 4;
            break;
        case 10:
            value = 11;
            break;
        case 11:
            value = 6;
            break;
        case 12:
            value = 1;
            break;
        case 13:
            value = 5;
            break;
        case 14:
            value = 12;
            break;
        case 15:
            value = 7;
            break;
        case 16:
            value = 2;
            break;
        case 17:
            value = 9;
            break;
        case 18:
            value = 4;
            break;
        case 19:
            value = 11;
            break;
        case 20:
            value = 6;
            break;
        case 21:
            value = 1;
            break;
        case 22:
            value = 8;
            break;
        case 23:
            value = 3;
            break;
        case 24:
            value = 10;
            break;
        case -1:
            break;
        default:
            jassert(false); // chromaKey not recognised
    }
}


int KeySorter::compareElements(TrackInfo* first, TrackInfo* second)
{
    // Get the compatibility of each key with the reference key
    int compatibilityFirst = reference.compability(CamelotKey(first->key));
    int compatibilitySecond = reference.compability(CamelotKey(second->key));
    
    // Compare the compatibility ratings
    // JUCE element comparator architecture dictates that:
    // -1 if first element sorts higher, 1 if second element sorts higher, 0 if it's a draw
    if (compatibilityFirst > compatibilitySecond)
        return -1;
    else if (compatibilitySecond > compatibilityFirst)
        return 1;
    
    return 0;
}
