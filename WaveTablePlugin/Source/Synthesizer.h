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
    SynthVoice(std::vector<double>& table, int tSize);
    
    bool canPlaySound(juce::SynthesiserSound*) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    
    void renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
    double interpNextSamp() noexcept;
    void updateAngle();
    
    
    
    //Unused Pure Virtual Functions
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    
    
private:
    double currentIndex;
    double m_angle;
    double m_angleDelta;
    double m_level;
    double m_freq;
    
    std::vector<double>& m_table;
    unsigned int m_tableSize;
};

// JUCE AudioSource class provides basic structure for audio processing commands for the Plugin Processor
class SynthAudioSource : public juce::AudioSource
{
public:
    SynthAudioSource(juce::MidiKeyboardState& keyState);
    ~SynthAudioSource();
    
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &buffertoFill) override;
    void generateWavetable(std::vector<double>& bufferToFill, unsigned int size);
    
    
    
private:
    juce::Synthesiser synth;
    juce::MidiKeyboardState& m_keyState;
    unsigned int maxVoices = 8;
    unsigned int defaultTableSize = 1 << 11;
    std::vector<double> m_table;
};







