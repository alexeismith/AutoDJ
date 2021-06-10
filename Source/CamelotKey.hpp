//
//  CamelotKey.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 09/05/2021.
//

#ifndef CamelotKey_hpp
#define CamelotKey_hpp

#include <JuceHeader.h>
#include "TrackInfo.hpp"


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
    
    /** Generates the name of this key.
     
     @return Camelot key name */
    juce::String getName();
    
    /** Fetches the colour of this key.
    
    @return Camelot key colour */
    juce::Colour getColour();
    
    
private:
    
    /** Convert a chromatic key number to Camelot representation. Chromatic key is used for database storage, rather than Camelot, because it is a simpler representation.
     
     Chromatic key is in the range 1-24, where 1-12 is major and 13-24 is minor, and the tonic notes ascend chromatically.
     E.g: 1 = C, 2 = Db, 13 = Cm, 14 = Dbm
     
     @param[in] chromaKey Chromatic input value
     */
    void fromChromaKey(int chromaKey);
    
    int value; ///< Camelot number, indicating the tonic of this key signature (see Camelot wheel)
    bool major; ///< Mode of this key signature
    
    // Give KeySorter access to private member variables
    friend class KeySorter;
    
};


/**
 Simple class for sorting tracks, in terms of their key's compatibility with a reference key.
 Conforms to JUCE's element comparator template:
 https://docs.juce.com/master/tutorial_table_list_box.html#tutorial_table_list_box_sorting_data
 */
class KeySorter
{
public:
    
    /** Constructor - sets the key to compare against for the compareElements() function. */
    KeySorter(CamelotKey referenceKey) : reference(referenceKey) {}
    
    /** Destructor. */
    ~KeySorter() {}
    
    /** Element comparator function.
    
    @param[in] first Pointer to first track to be compared
    @param[in] second Pointer to second track to be compared
    
    @return -1 if first element sorts higher, 1 if second element sorts higher, 0 if it's a draw */
    int compareElements(TrackInfo* first, TrackInfo* second);
    
    /** Sorts the two provided keys in terms of their names.
     (Ascending Camelot number, and minor before major)
     
     @param[in] first Pointer to first track to be compared
     @param[in] second Pointer to second track to be compared
     
     @return -1 if first element sorts higher, 1 if second element sorts higher, 0 if it's a draw */
    static int sortByName(CamelotKey first, CamelotKey second);
    
private:
    
    CamelotKey reference; ///< Reference key to sort elements against

};

#endif /* CamelotKey_hpp */
