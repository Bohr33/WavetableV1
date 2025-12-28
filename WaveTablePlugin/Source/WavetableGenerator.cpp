/*
  ==============================================================================

    WavetableGenerator.cpp
    Created: 23 Dec 2025 7:44:30pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

#include "WavetableGenerator.h"
#include <algorithm>


WavetableGenerator::WavetableGenerator()
{};

WavetableGenerator::~WavetableGenerator() = default;

void WavetableGenerator::normalizeTable(std::span<float> buffer, float normValue = 1.0)
{
    size_t N = buffer.size();
    jassert(N > 0);
    
    //Find Max Value
    float peak = 0.0;
    for (auto i = 0; i < N; i++)
        peak = std::max(peak, std::abs(buffer[i]));
    
    //Scale all values based on ratio to target value
    if(peak > 0.0)
    {
        float ratio =  peak/normValue;
        for (auto k = 0; k < N; k++)
            buffer[k] /= ratio;
    }
}


void WavetableGenerator::additiveGenerator(std::span<float> bufferToFill, std::span<Partial> partials, size_t guardPoints = 1)
{
    
    //Get Size, factor in guard point
    size_t N = bufferToFill.size() - guardPoints;
    jassert(N > 0);
    
    size_t numPartials = partials.size();
    
    //Prepare Sine Variables for Loop
    float incr = 0.0;
    float angle = 0.0;
    
    juce::Logger::writeToLog("Num Partials = " + juce::String(numPartials));
    
    //Loop and add value to vector
    for(auto i = 0; i < numPartials; i++)
    {
        Partial p = partials[i];
        auto harmonic = p.harmonic;
        
        incr = harmonic / (float)N;
        angle = p.phase;
        
        for(auto i = 0; i < N; ++i)
        {
            auto val = std::sin(juce::MathConstants<float>::twoPi * angle) * p.amplitude;
            bufferToFill[i] += val;
            angle += incr;
        }
    }
    
    //fill guard point(s)
    for (auto i = 0; i < guardPoints; i++)
        bufferToFill[N + i] = bufferToFill[i];
    
}


void WavetableGenerator::genSine(std::span<float> bufferToFill, int numHarmonics)
{
    
    Partial sine{1.0f, 1.0f, 0.0f};
    
    //Cast as span so we can pass to Generator
    std::span<Partial> span(&sine, 1);
    
    additiveGenerator(bufferToFill, span);
}


void WavetableGenerator::genSaw(std::span<float> bufferToFill, int numHarmonics)
{
    
    std::vector<Partial> sawPartials;
    sawPartials.resize(numHarmonics);
    
    for (auto i = 0; i < numHarmonics; i++) {
        sawPartials[i].harmonic = i + 1.0f;
        sawPartials[i].amplitude = 1.0f/(i+ 1.0f);
        sawPartials[i].phase = 0.0f;
    }
    
    additiveGenerator(bufferToFill, sawPartials);
    normalizeTable(bufferToFill);
}

void WavetableGenerator::genTri(std::span<float> bufferToFill, int numHarmonics)
{
    
    std::vector<Partial> partials;
    partials.resize(numHarmonics);
    
    for (auto i = 0; i < numHarmonics; i++) {
        float n = 1.0f + 2.0f * i;
        float sign = (i % 2 == 0) ? 1.0f : -1.0f;
        partials[i].harmonic = n;
        partials[i].amplitude = 1.0f/std::pow(n, 2.0f) * sign;
        partials[i].phase = 0.0f;
    }
    
    additiveGenerator(bufferToFill, partials);
    normalizeTable(bufferToFill);
}


void WavetableGenerator::genSquare(std::span<float> bufferToFill, int numHarmonics)
{
    
    std::vector<Partial> partials;
    partials.resize(numHarmonics);
    
    for (auto i = 0; i < numHarmonics; i++) {
        float n = 1.0f + i * 2.0f;
        partials[i].harmonic = n;
        partials[i].amplitude = 1.0f/n;
        partials[i].phase = 0.0f;
    }
    
    additiveGenerator(bufferToFill, partials);
    normalizeTable(bufferToFill);
}
