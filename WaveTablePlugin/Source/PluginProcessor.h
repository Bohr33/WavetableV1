/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Synthesizer.h"

//==============================================================================
/**
*/
class WaveTablePluginAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    WaveTablePluginAudioProcessor();
    ~WaveTablePluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    
    void generateWavetable(std::vector<double>& bufferToFill, unsigned int size);
    void generateSawtooth(std::vector<double>& bufferToFill, unsigned int size);
    
    
    //Added Functions
    juce::MidiKeyboardState& getMidiKeyboardState();

private:
    juce::MidiKeyboardState m_keystate;
    unsigned int bufferSize = 128;
    
    
    //Synth Parameters
    const int maxVoices = 8;
    const int defaultTableSize = 1 << 9;
    
    juce::Synthesiser synth;
    std::vector<double> m_table;
    std::vector<double> m_table2;
    juce::MidiMessageCollector midiCollector;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveTablePluginAudioProcessor)
};
