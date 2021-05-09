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
    
    if (key.value == value)
    {
        if (key.major == major)
            return 2;
        else
            return 1;
    }
    else if (key.major == major)
    {
        if (abs(key.value - value) == 1)
            return 1;
    }
    
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
    if (chromaKey > 12)
        major = false;
    else
        major = true;
    
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
