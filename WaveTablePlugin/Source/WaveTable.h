/*
  ==============================================================================

    WaveTable.h
    Created: 26 Dec 2025 9:39:19pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


struct MipMap
{
    std::vector<std::vector<float>> stages;
    
    int numMipMaps;
    
  
    const std::vector<std::vector<float>>& getMipMap() const
    {
        return stages;
    }
    
    const std::vector<float>& getStage(int index) const
    {
        return stages[index];
    }
    
    size_t size() const {
        return stages.size();
    }
    
    
};

