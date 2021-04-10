//
//  AudioProcessor.hpp
//  AutoDJ - App
//
//  Created by Alexei Smith on 09/04/2021.
//

#ifndef AudioProcessor_hpp
#define AudioProcessor_hpp

#include <JuceHeader.h>

class AudioProcessor
{
public:
    
    AudioProcessor();
    
    ~AudioProcessor() {}
    
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    
    void play(juce::AudioBuffer<float> audio);
    
    
private:
    
    std::atomic<bool> audioReady;
    juce::AudioBuffer<float> audioBuffer;
    int playhead = -1;
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioProcessor)
};

#endif /* AudioProcessor_hpp */
