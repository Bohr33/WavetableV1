/*
  ==============================================================================

    Synthesizer.h
    Created: 25 Nov 2025 4:54:55pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "WaveTable.h"
#include <stdio.h>
#include <vector>
#include <span>

class WaveTableSound : public juce::SynthesiserSound
{
public:
    WaveTableSound();
    bool appliesToNote(int midiNoteNumber) override;
    bool appliesToChannel(int midiChannel) override;
    
private:
};

class SynthVoice : public juce::SynthesiserVoice
{
public:
//    SynthVoice(std::vector<float>& table, std::vector<float>& table2, int tSize);
    SynthVoice(std::shared_ptr<const TableData> initialTable, int tableSize);
    
    bool canPlaySound(juce::SynthesiserSound*) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    
    void renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
//    float interpNextSamp() noexcept;
//    float interpNextSamp(std::vector<float>& table) noexcept;
    
    
    void updateAngle();
    
    //Unused Pure Virtual Functions
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    
    float interpolateValue(float val);
    
    float interpolate(float interp_val, std::shared_ptr<const TableData> tableOne, std::shared_ptr<const TableData> tableTwo);
    float interpNextSamp2(std::shared_ptr<const TableData> table) noexcept;
    
    void setParameters(std::atomic<float>* param);
    
    void setWavetable(int tableID, std::shared_ptr<const TableData> newTable);
    
    
    
    void printTable();
    
    
private:
    const float tailDecayTime = 1.0;
    
    float currentIndex;
    float m_angle;
    float m_angleDelta;
    float m_level;
    float m_freq;
    float m_tail;
    float m_tailDec = 0.99;
    
    std::atomic<float>* interpParam = nullptr;
    
//    std::vector<float>& m_table;
//    std::vector<float>& m_table2;
    unsigned int m_tableSize;
    
    //Shared pointer to currently chosen table
    std::shared_ptr<const TableData> m_tableOne;
    std::shared_ptr<const TableData> m_tableTwo;
    
    int test = 0;
};







