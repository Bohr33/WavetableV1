/*
  ==============================================================================

    WavetableGenerator.cpp
    Created: 23 Dec 2025 7:44:30pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

#include "WavetableGenerator.h"


WavetableGenerator::WavetableGenerator()
{};

WavetableGenerator::~WavetableGenerator() = default;

void WavetableGenerator::generateSine(std::vector<double>& bufferToFill)
{
    jassert(!bufferToFill.empty());
    auto size = bufferToFill.size();
    
    //Prepare Sine Variables for Loop
    double incr = 1.0 / (double)size;
    double angle = 0.0;

    //Loop and add value to vector
    for(auto i = 0; i < size; ++i)
    {
        auto val = std::sin(angle * juce::MathConstants<double>::twoPi);
        bufferToFill[i] = val;
        angle += incr;
    }
    //Guard Point
    bufferToFill[size] = bufferToFill[0];
};

//Sawtooth Generation Function
void WavetableGenerator::generateSaw(std::vector<double>& bufferToFill)
{
    jassert(!bufferToFill.empty());
    auto size = bufferToFill.size();
    
    //Arbitrary Value of Harmonics
    int numHarmonics = 10;
    
    //Prepare Sine Variables for Loop
    double incr = 0.0;
    double angle = 0.0;

    //Loop and add value to vector
    for (auto harmonic = 1; harmonic <= numHarmonics; harmonic++) {
        
        incr = (double)harmonic / (double)size;
        angle = 0.0;
        
        for(auto i = 0; i < size; ++i)
        {
            auto val = std::sin(angle * juce::MathConstants<double>::twoPi) / (double)harmonic;
            bufferToFill[i] += val;
            angle += incr;
        }
        
    }
    
    
    //Normalize Wave
    auto peak = 0.0;
    for (auto j = 0; j < size; j++)
        peak = std::max(peak, std::abs(bufferToFill[j]));
    
    if(peak > 0.0)
        for (auto k = 0; k < size; k++)
            bufferToFill[k] /= peak;

    //Guard Point
    bufferToFill[size] = bufferToFill[0];
    
    
}
