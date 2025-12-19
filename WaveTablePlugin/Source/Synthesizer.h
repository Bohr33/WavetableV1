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
    WaveTableSound(std::vector<double>& table);
    bool appliesToNote(int midiNoteNumber) override;
    bool appliesToChannel(int midiChannel) override;
    std::vector<double>& getTable();
    
private:
    std::vector<double>& m_table;
};

class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice(std::vector<double>& table, std::vector<double>& table2, int tSize);
    
    bool canPlaySound(juce::SynthesiserSound*) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    
    void renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
    double interpNextSamp() noexcept;
    double interpNextSamp(std::vector<double>& table) noexcept;
    void updateAngle();
    
    //Unused Pure Virtual Functions
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    
    double interpolateValue(float val);
    
    void setParameters(std::atomic<float>* param);
    
    
private:
    const double tailDecayTime = 1.0;
    
    double currentIndex;
    double m_angle;
    double m_angleDelta;
    double m_level;
    double m_freq;
    double m_tail;
    double m_tailDec = 0.99;
    
    std::atomic<float>* interpParam = nullptr;
    
    std::vector<double>& m_table;
    std::vector<double>& m_table2;
    unsigned int m_tableSize;
};







