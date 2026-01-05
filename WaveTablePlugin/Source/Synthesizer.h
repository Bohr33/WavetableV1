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
//    SynthVoice(std::shared_ptr<const TableData> tableOne, std::shared_ptr<const TableData> tableTwo, std::shared_ptr<const MipMap> mipmapA, std::shared_ptr<const MipMap> mipmapB, int tableSize);
    SynthVoice(std::shared_ptr<const MipMap> mipmapA, std::shared_ptr<const MipMap> mipmapB, int tableSize);
    
    bool canPlaySound(juce::SynthesiserSound*) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
    
    void pitchWheelMoved(int newPitchWheelValue) override {
        m_pitchBend = newPitchWheelValue;
        juce::Logger::writeToLog("New Pitch Value: " + juce::String(m_pitchBend));
    }
    
    
    
    //Sets ADSR Sample Rate, but can do other stuff...
    void prepare(double sampleRate);
    
    
    //Unused Pure Virtual Functions
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    
    
    //Rendering Functions
    void setFrequency(float frequency);
    void updateAngle();
    
    float interpNextSamp(std::shared_ptr<const TableData> table) noexcept;
    float interpNextSamp(std::vector<float>& table);
    float interpolate(float interp_val, std::shared_ptr<const TableData> tableOne, std::shared_ptr<const TableData> tableTwo);
    float interpolate(float interp_val, float val1, float val2);
    
    //Setter Functions
    void setAPVTS(juce::AudioProcessorValueTreeState* apvts);
    void setWavetable(int tableID, std::shared_ptr<const TableData> newTable);
    void setMipMap(int displayID, std::shared_ptr<const MipMap> newMipMap);
    
    //PitchBend
    void updateFrequency();
    
    
    
    //Misc Debug Functions
    void printTable();
    void reportTables();
    void reportMipMaps();
    
    
private:
    
    float calculateBendFreq();
    
    juce::AudioProcessorValueTreeState* apvtsRef = nullptr;

    float currentIndex;
    float m_angle;
    float m_angleDelta;
    float m_level;
    float m_baseFreq;
    float m_currentFreq;
    
    int m_pitchBend = 8192;
    float m_pitchBendRange = 2.0;
    
    
    std::atomic<float>* interpParam = nullptr;
    
    std::atomic<float>* attackParam = nullptr;
    std::atomic<float>* decayParam = nullptr;
    std::atomic<float>* sustainParam = nullptr;
    std::atomic<float>* releaseParam = nullptr;
    
    std::atomic<float>* attCurveParam = nullptr;
    std::atomic<float>* decCurveParam = nullptr;
    std::atomic<float>* relCurveParam = nullptr;
    

    unsigned int m_tableSize;
    
    ADSR envelope;
    
    //Shared pointer to currently chosen table
    std::shared_ptr<const TableData> m_tableOne;
    std::shared_ptr<const TableData> m_tableTwo;
    
    int m_mapStage;
    
    //Shared Pointers to Mipmaps
    std::shared_ptr<const MipMap> m_mipmapA;
    std::shared_ptr<const MipMap> m_mipmapB;
    
    
    
    int test = 0;
    
};







