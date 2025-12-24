/*
  ==============================================================================

    Synthesizer.h
    Created: 25 Nov 2025 4:54:55pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <stdio.h>
#include <vector>

class WaveTableSound : public juce::SynthesiserSound
{
public:
    WaveTableSound(std::vector<float>& table);
    bool appliesToNote(int midiNoteNumber) override;
    bool appliesToChannel(int midiChannel) override;
    std::vector<float>& getTable();
    
private:
    std::vector<float>& m_table;
};

class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice(std::vector<float>& table, std::vector<float>& table2, int tSize);
    
    bool canPlaySound(juce::SynthesiserSound*) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    
    void renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
    float interpNextSamp() noexcept;
    float interpNextSamp(std::vector<float>& table) noexcept;
    void updateAngle();
    
    //Unused Pure Virtual Functions
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    
    float interpolateValue(float val);
    
    void setParameters(std::atomic<float>* param);
    
    
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
    
    std::vector<float>& m_table;
    std::vector<float>& m_table2;
    unsigned int m_tableSize;
};







