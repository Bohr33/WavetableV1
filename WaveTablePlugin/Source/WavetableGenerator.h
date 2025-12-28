/*
  ==============================================================================

    WavetableGenerator.h
    Created: 23 Dec 2025 7:44:30pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/


#include <JuceHeader.h>
#include <vector>
#include <span>
#pragma once

struct Partial
{
    float harmonic;
    float amplitude;
    float phase;
};

class WavetableGenerator
{
public:
    WavetableGenerator();
    ~WavetableGenerator();
    
    void additiveGenerator(std::span<float> bufferToFill, std::span<Partial> partials, size_t guardPoints);
    
    void normalizeTable(std::span<float> buffer, float normValue);
    
    
    void genSine(std::span<float> bufferToFill, int numHarmonics);
    void genSaw(std::span<float> bufferToFill, int numHarmonics);
    void genTri(std::span<float> bufferToFill, int numHarmonics);
    void genSquare(std::span<float> bufferToFill, int numHarmonics);
    
private:
    
    
    
};

