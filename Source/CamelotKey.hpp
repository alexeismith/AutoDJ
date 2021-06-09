//
//  CamelotKey.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 09/05/2021.
//

#ifndef CamelotKey_hpp
#define CamelotKey_hpp

#include <JuceHeader.h>


/**
 Represents key signature using the Camelot wheel - see: https://mixedinkey.com/harmonic-mixing-guide/
 */
class CamelotKey
{
public:
    
    /** Constructor. */
    CamelotKey(int chromaKey) { fromChromaKey(chromaKey); }
    
    /** Destructor. */
    ~CamelotKey() {}
    
    /** Checks the compatibility of the provided key with this one.
     
     @param[in] key Camelot key to check against this one
     
     @return 2: keys are equal, 1: keys are not equal, but still compatible, 0: keys are not compatible */
    int compability(CamelotKey key);
    
    /** Sorts the two provided keys in terms of compatibility with this key.
     
     @param[in] first Pointer to first track to be compared
     @param[in] second Pointer to second track to be compared
     
     @return -1 if first element sorts higher, 1 if second element sorts higher, 0 if it's a draw */
    static int sort(CamelotKey first, CamelotKey second);
    
    /** Generates the name of this key.
     
     @return Camelot key name */
    juce::String getName();
    
    /** Fetches the colour of this key.
    
    @return Camelot key colour */
    juce::Colour getColour();
    
    
private:
    
    /** Sets this key from a chromatic key input value
     
     @param[in] chromaKey Chromatic input value
     */
    void fromChromaKey(int chromaKey);
    
    int value = -1; ///< Camelot number, indicating the tonic of this key signature (see Camelot wheel)
    bool major; ///< Mode of this key signature
    
};

#endif /* CamelotKey_hpp */
