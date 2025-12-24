/*
  ==============================================================================

    WavetableGenerator.h
    Created: 23 Dec 2025 7:44:30pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/


#include <JuceHeader.h>
#include <vector>
#pragma once

class WavetableGenerator
{
public:
    WavetableGenerator();
    ~WavetableGenerator();
    
    void generateSine(std::vector<double>& bufferToFill);
    void generateSaw(std::vector<double>& bufferToFill);
    
    
private:
    
    
    
};

