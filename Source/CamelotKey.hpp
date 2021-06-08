//
//  CamelotKey.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 09/05/2021.
//

#ifndef CamelotKey_hpp
#define CamelotKey_hpp

#include <JuceHeader.h>


class CamelotKey
{
public:
    
    /** Constructor. */
    CamelotKey(int chromaKey) { fromChromaKey(chromaKey); }
    
    /** Destructor. */
    ~CamelotKey() {}
    
    int compability(CamelotKey key);
    
    static int sort(CamelotKey first, CamelotKey second);
    
    juce::String getName();
    
    juce::Colour getColour();
    
    
private:
    
    void fromChromaKey(int chromaKey);
    
    int value = -1;
    bool major;
    
};

#endif /* CamelotKey_hpp */
