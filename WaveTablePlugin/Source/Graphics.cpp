/*
  ==============================================================================

    Graphics.cpp
    Created: 20 Dec 2025 9:39:23pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

#include "Graphics.h"

WavetableDisplay::WavetableDisplay(){};
WavetableDisplay::~WavetableDisplay() = default;

////void WavetableDisplay::setTable(std::vector<double>* table)
//{
////    wavetable = table;
//}

void WavetableDisplay::reDraw()
{
    
}

void WavetableDisplay::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::red);
    g.drawRect(getLocalBounds());
    
}

