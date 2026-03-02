/*
  ==============================================================================

    ImportWavetable.cpp
    Created: 28 Feb 2026 8:16:52pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

#include "ImportWavetable.h"

// 1. Load file, read frame size if available, if not find and store
// 2. read file, parse to find single cycle if not defined
// 3. send result to generate mip map
// 4. store the generated mipmap into wavetable bank




void importWavetable(const juce::File& file)
{
    //CHeck frame size
    
    //asssume frame size is 2048 now
    int frameSize = 2048;
    
    
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
    if (reader == nullptr) return;

    juce::AudioBuffer<float> buffer(1, (int)reader->lengthInSamples);
    reader->read(&buffer, 0, (int)reader->lengthInSamples, 0, true, false);
    
    int numFrames = buffer.getNumSamples() / frameSize;
    
    for( int i = 0; i < numFrames; ++i)
    {
        auto ptr = buffer.getReadPointer(0, i * numFrames);
        
        
        
    }
    

    
    
    
    
    
    
}


void getFrameSize(const juce::File& file)
{
    //1. check for
    
    
}
