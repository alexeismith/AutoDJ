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
    return juce::String(floor(secs / 60)) + ":" + juce::String(secs % 60);
}

} /* namespace AutoDJ */
