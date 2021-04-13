//
//  CommonDefs.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 01/03/2021.
//

#ifndef CommonDefs_hpp
#define CommonDefs_hpp

#include <JuceHeader.h>

#define SUPPORTED_SAMPLERATE (44100)
#define TRACK_LENGTH_SECS_MIN (60)
#define TRACK_LENGTH_SECS_MAX (600)
#define BEATS_PER_BAR (4)

enum ComponentIDs : int
{
    chooseFolder
};

namespace AutoDJ {

juce::String getLengthString(int secs);

} /* namespace AutoDJ */

#endif /* CommonDefs_hpp */
