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
#include "ADSR.h"

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
    SynthVoice(std::shared_ptr<const TableData> tableOne, std::shared_ptr<const TableData> tableTwo, int tableSize);
    
    bool canPlaySound(juce::SynthesiserSound*) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
    
    
    //Sets ADSR Sample Rate, but can do other stuff...
    void prepare(double sampleRate);
    
    
    //Unused Pure Virtual Functions
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    
    
    //Rendering Functions
    void updateAngle();
    
    float interpNextSamp(std::shared_ptr<const TableData> table) noexcept;
    float interpolate(float interp_val, std::shared_ptr<const TableData> tableOne, std::shared_ptr<const TableData> tableTwo);
    float interpolate(float interp_val, float val1, float val2);
    
    //Setter Functions
    void setParameters(std::atomic<float>* param);
    void setWavetable(int tableID, std::shared_ptr<const TableData> newTable);
    
    
    //Misc Debug Functions
    void printTable();
    void reportTables();
    
    
private:

    float currentIndex;
    float m_angle;
    float m_angleDelta;
    float m_level;
    float m_freq;
    
    
    std::atomic<float>* interpParam = nullptr;

    unsigned int m_tableSize;
    
    ADSR envelope;
    
    //Shared pointer to currently chosen table
    std::shared_ptr<const TableData> m_tableOne;
    std::shared_ptr<const TableData> m_tableTwo;
    
};







