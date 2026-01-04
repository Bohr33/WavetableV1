/*
  ==============================================================================

    WaveTable.h
    Created: 26 Dec 2025 9:39:19pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct TableData
{
    std::vector<float> samples;
    
    
    const std::vector<float>& getTable() const
    {
        return samples;
    }
    
    
    const float* getData() const {
        return samples.data();
    }
    
};

